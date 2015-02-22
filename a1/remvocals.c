#include <stdio.h>
#include <stdlib.h>

	#define HEADER_SIZE 44
	#define HEADER_SIZE_Short 22

	int main(int argc, char **argv) {
		//input and output files
		FILE *sourcewav, *destwav;
		//size of file in bytes
		long size;
		//size of file in shorts
		long sSize;
		//loop iteration variables
		int i, j;
		
		//buffer to store bytes
		char *buffer = NULL;

		//checks whether correct number of arguments
		if(argc !=3) {
			fprintf(stderr, "Usage: %s <file>/n", argv[0]);
			exit(1);
		}

		//opens input file and checks for errors
		sourcewav = fopen(argv[1], "rb");
		if(sourcewav  == NULL) {
			perror(argv[1]);
			exit(1);
		}
		//opens output file and checks for errors
		destwav = fopen(argv[2], "wb");
		if(destwav == NULL) {
			perror(argv[2]);
			exit(1);
		}
		//Find the size of sourcewav
		if(fseek(sourcewav, 0L, SEEK_END)!=0){
			printf("Error find position in file.\n");
			exit(1);
		} 

		size = ftell(sourcewav);

		buffer = malloc(sizeof(char) * (size + 1));
		//sets up array of shorts
		short outFile[size+1];
		sSize = (size/2)-HEADER_SIZE_Short;
		short s[sSize+1];
		//Returns the position of sourcewav to the first byte
		if(fseek(sourcewav, 0L, SEEK_SET) != 0) {
			printf("Error finding position in file.\n");
			exit(1);
		}

		//reads the first 44 bytes of the source file to a buffer
		if(fread(buffer, sizeof(char), HEADER_SIZE, sourcewav) != HEADER_SIZE) {
			printf("Error reading header.\n");
			exit(1);
		}
		
		//writes the first 44 bytes of the source file to the output file
		if(fwrite(buffer, sizeof(char), HEADER_SIZE, destwav) != HEADER_SIZE) {
			printf("Error writing header.\n");
			exit(1);
		}
		//sets the position of the file to the 44 byte
		if(fseek(sourcewav, HEADER_SIZE, SEEK_SET) != 0) {
			printf("Error finding position in file.");
			exit(1);
		}
		
		//Reads the rest of sourcewav into a array of shorts
		for(i = 0; i<sSize; i++){
			if(fread(&s[i], sizeof(short), 1, sourcewav) != 1) {
				printf("Error reading input file.\n");
				exit(1);
			}
		}
		//Removes vocals
		for(i = 0; i<sSize; i=i+2) {
			for(j = i; j< i+ 1; j++) {
			
				outFile[j] = (s[j] - s[j+1])/2;
				outFile[j+1] = outFile[j];
			}

		}
		//writes modified shorts to destwav
		if(fwrite(outFile, sizeof(short), sSize, destwav)!= sSize) {
			printf("Error writing to output file.\n");
			exit(1);
		}
		//closes files
		fclose(sourcewav);
		fclose(destwav);

	return 0;
	}
