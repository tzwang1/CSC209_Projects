#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HEADER_SIZE 22 
#define HEADER_SIZE_BYTES 44
#define DEFAULT_DELAY 8000
#define DEFAULT_VOLUME 4

	int main(int argc, char **argv){

		//array to store header
		short header[HEADER_SIZE+1];
		//Used for getopt
		int option;
		int index;
		//size of input file in bytes
		long size;
		//size of input file in shorts
		long sSize;
		//used to help iterate through loops
		int i;
		int a = 0;
		int j, k;
		//number of times echo buffer needs to be read and write
		int numRead;
		//number of left over samples
		int leftOverSamples;
		//number of times output file has been written to
		int numWrite = 0;
		//input and output files
		FILE *sourcewav, *destwav;
		//default values for delay and volumeScale
		long int delay = DEFAULT_DELAY, volumeScale = DEFAULT_VOLUME;
		//array to store original samples/mixed samples
		short *fileBuffer;
		
		unsigned int *sizeptr;
		//while loop to handle inputs
		while((option = getopt(argc, argv, "d:v:")) != -1) {
			switch(option) {
				//case to handle delay
				case 'd' : 
								delay = strtol(optarg, NULL, 10);
								if(delay < 1){
									printf("Error, delay must be a positive integer\n");
									exit(1);
								}
								printf("The delay is: %ld\n", delay);
								
					break;
				//case to handle volume
				case 'v' : 
								volumeScale = strtol(optarg, NULL, 10);
								if(volumeScale < 1){
									printf("Error, volume scale must be a positive integer\n");
									exit(1);
								}
								printf("The volume is: %ld\n", volumeScale);
					break;
				
				case '?':
					printf("Error\n");
					abort();
				break;
			}
		}
	
		index = optind;
		//handles input file argument
		sourcewav = fopen(argv[index], "rb");
		if(sourcewav  == NULL) {
			perror(argv[index]);
			exit(1);
		}
		//handles output file argument
		index = index+1;
		destwav = fopen(argv[index], "wb");
		if(destwav  == NULL) {
			perror(argv[index]);
			exit(1);
		}
			
		
		//read header in to buffer	
		for(i = 0; i<HEADER_SIZE; i++){
			if(fread(&header[i], sizeof(short), 1, sourcewav)!= 1){
				printf("Error in reading header\n");
				exit(1);
			}	
		}

		//Uses a pointer to increase short 2 by delay*2
		sizeptr = (unsigned int *)(header + 2);
		*sizeptr = *sizeptr + delay *2;
		//Uses a pointer to increase short 20 by delay*2
		sizeptr = (unsigned int *)(header + 20);
		*sizeptr = *sizeptr + delay *2;
		
		//writes header into destwav
		if(fwrite(header, sizeof(short), HEADER_SIZE, destwav) != HEADER_SIZE){
			printf("Error\n");
		}
		
		if(fseek(sourcewav, 0L, SEEK_END) != 0) {
			printf("Error finding position of file\n");
			exit(1);
		}
		//determines the size of the input file in bytes 
		size = ftell(sourcewav); 
		//determines the number of samples in the file
		sSize = (size/2)-HEADER_SIZE;
		
		//determine size of file buffer
		fileBuffer = malloc(sizeof(short) * (size + 1));
		
		//number of times to read bytes into echoBuffer
		numRead = sSize/delay;
		
		if(fseek(sourcewav, HEADER_SIZE_BYTES, SEEK_SET) != 0) {
			printf("Error finding position of file\n");
			exit(1);
		}
		//reads input file into file buffer
		if(fread(fileBuffer, sizeof(short), sSize, sourcewav) != sSize) {
			printf("Error reading shorts into file buffer\n");
			exit(1);
		}
		//writes file buffer to output file
		if(fwrite(fileBuffer, sizeof(short), delay, destwav) != delay) {
			printf("Error writing shorts to output file\n");
			exit(1);
		}
	
		//while loop to handle mixing
		while(a<numRead) {
			//if delay is greater than number of samples write input file to output file and exit program
			if(delay > sSize) {
				if(fseek(destwav, HEADER_SIZE_BYTES, SEEK_SET) != 0) {
					printf("Error find position in file\n");
					exit(1);
				}
				if(fwrite(fileBuffer, sizeof(short), sSize, destwav) != sSize) {
					printf("Error writing file buffer to output file\n");
					exit(1);
				}
				printf("Cannot add echo, delay is greater than number of samples\n");
				exit(1);
			}
	
			//Creates new echo buffer of delay size
			short *echoBuffer = malloc(sizeof(short) * delay);
		
			//Finds the position to start reading bytes into the echoBuffer
			if(a*delay <= sSize) {
				if(fseek(sourcewav, HEADER_SIZE_BYTES+(delay*numWrite), SEEK_SET)!=0) {
					printf("Error finding position in file\n");
					exit(1);
				}
				//reads delay samples into echo buffer
				if(fread(echoBuffer, sizeof(short), delay, sourcewav) != delay) {
					printf("Error reading delay number of samples into echo buffer\n");
					exit(1);
				}
			
				//scales volume of samples in echoBuffer
				for(j = 0; j<sizeof(echoBuffer); j++) {
					echoBuffer[j] = echoBuffer[j]/volumeScale;
					
				}
			
				//starts mixing fileBuffer and echoBuffer, after delay samples
				for(k = 0; k<delay; k++){
					fileBuffer[k+(delay*(numWrite+1))] = fileBuffer[k+(delay*(numWrite+1))] + echoBuffer[k];
					
				}
			} 
			
			numWrite++;
			a++;
			
		}
		//handles left over samples
		short *echoBuffer = malloc(sizeof(short) * delay);
		leftOverSamples = (sSize) - (numRead)*delay; 

		
		for(i = 0; i< leftOverSamples; i++) {
					if(fseek(sourcewav, HEADER_SIZE_BYTES+(delay*numWrite), SEEK_SET)!= 0){
						printf("Error finding correct position of file\n");
						exit(1);
					}
					if(fread(echoBuffer, sizeof(short), leftOverSamples, sourcewav)!= leftOverSamples) {
						printf("Error reading left over samples into echo buffer\n");
						exit(1);
					}
				}
				//scales volume of samples in echo buffer
				for(j = 0; j<sizeof(echoBuffer); j++) {
					echoBuffer[j] = echoBuffer[j]/volumeScale;
					
				}
				//mixes samples in echo buffer with samples in file buffer
				for(k = 0; k<leftOverSamples; k++){
					fileBuffer[k+(delay*(numWrite))] = fileBuffer[k+(delay*(numWrite))] + echoBuffer[k];
				}
		
		if(fseek(destwav, HEADER_SIZE_BYTES+delay, SEEK_SET)!=0) {
			printf("Error finding position in file\n");
			exit(1);
		}
		
		if(fwrite(fileBuffer, sizeof(short), sSize, destwav) != sSize){
			printf("Error writing file buffer to output file\n");\
			exit(1);
		}
		
		fclose(sourcewav);
		fclose(destwav);
		
		return 0;
	}
	
