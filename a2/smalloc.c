#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

void *mem;
struct block *freelist;
struct block *allocated_list;

void *smalloc(unsigned int nbytes) {
	//checks whether the size of freelist is large enough to store nbytes

	struct block *freelist_pointer;
	freelist_pointer = freelist;
	
	
	while(freelist_pointer != NULL){

		if(nbytes < freelist_pointer->size) {
		
			//first allocation
			if(allocated_list == NULL) {
			
				struct block *new_node;
				
				new_node = malloc(sizeof(struct block));
				new_node->addr = freelist_pointer->addr;					
				new_node->size = nbytes;
									
				freelist_pointer->addr = new_node->addr+nbytes;		
				freelist_pointer->size = freelist->size-nbytes;

				new_node->next = allocated_list;
				allocated_list = new_node; 	
					
				return new_node->addr;
			}
			//second and up allocations
			else if(allocated_list != NULL)
			{
				
									printf("second and up allocations\n");			
				//creates the new node to be added to allocated_list
				struct block *new_node;	
				//Makes new node point to a block in freelist
				new_node = mem;
				//Makes address of new node to be address of allocated list + nbytes	
				new_node->addr = allocated_list->addr+nbytes;
				//sets the size of the new node
				new_node->size = nbytes;
				
									//printf("new node size in second and up = %d\n", new_node->size);
				//reduces size of freelist;
				freelist_pointer->size = freelist_pointer->size-nbytes;
									//printf("pointer size in second and up = %d\n", pointer->size);
				//moves the address of free list nbytes down the heap
				freelist_pointer->addr = freelist_pointer->addr+nbytes;
				
				//makes allocated_list point to new node

				new_node->next = allocated_list;
					
				allocated_list = new_node;
				
					//printf("pointer size 2 in second and up = %d\n", pointer->size);
				
					
					//printf("allocated_list size in second and up 1 = %d\n", allocated_list->size);
					//printf("allocated_list size in second and up 2 = %d\n", (allocated_list->next)->size);
					//printf("allocated_list size in second and up 3 = %d\n", ((allocated_list->next)->next)->size);
					//printf("allocated_list size in second and up 4 = %d\n", (((allocated_list->next)->next)->next)->size);
				
				return allocated_list;

			}
			
		}
		else if(nbytes == freelist_pointer->size){
			if(allocated_list == NULL) {
				struct block *new_node;

				new_node = mem;
				new_node->addr = freelist->addr;
				new_node->size = nbytes;
				new_node->next = allocated_list;

				allocated_list = new_node;

				freelist->addr = allocated_list->addr + nbytes;
				freelist->size = freelist_pointer->size-nbytes;
				freelist->next = NULL;
				
			}
		}
			
		if(nbytes > freelist_pointer->size) {
			freelist_pointer = freelist_pointer->next;
		}
		
	}
    return NULL;
}


int sfree(void *addr) {
	int i = 0;
	//creates a 2 temporary pointers
	struct block *pointer;
	struct block *pre_pointer;
	pointer = allocated_list->next;
	pre_pointer = allocated_list;
	//checks whether allocated list is null
	while((i = 0) && (pre_pointer != NULL)){
		//checks whether the address of allocated list is the same as addr, if not set allocated_list to point to 			the next value 
		if(pre_pointer->addr != addr && pointer->addr != addr){

			pre_pointer = pre_pointer->next;
			pointer = pointer->next;
		}
		//handles case when first block is not the block to free
		if(pointer->addr == addr){

			pre_pointer->next = pointer->next;

			//makes a new node in freelist
			struct block *new_node;
			new_node = pointer;
			new_node->next = freelist->next;
			freelist->next = new_node;

			//removes pointer from allocated list
			pointer = NULL;
			
			return 0;
			
		}
		//handles case when very first block is the one to remove
		if(pre_pointer->addr == addr){

			struct block *new_node;
			new_node = pre_pointer;
			new_node->next = freelist->next;
			freelist->next = new_node;

			//removes pointer from allocated list
			pre_pointer = NULL;

			return 0;
		}

	}
			
		
    return -1;
}

	
/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }


    freelist = malloc(size*sizeof(int));
    freelist->addr = mem;
    freelist->size = size;
    printf("freelist size = %d\n", freelist->size);
    freelist->next = NULL;

    allocated_list = NULL;


    /* NOTE: this function is incomplete */
        
}

void mem_clean(){

	struct block *pointer_free;
	struct block *pre_pointer_free;

	struct block *pointer_allo;
	struct block *pre_pointer_allo;

	pointer_free = freelist->next;
	pre_pointer_free = freelist;

	pointer_allo = allocated_list->next;
	pre_pointer_allo = allocated_list;

	//free memory for freelist
	while(pre_pointer_free != NULL){

		free(pre_pointer_free);
		pre_pointer_free = pointer_free;
		if(pointer_free->next != NULL){
			pointer_free = pointer_free->next;
		}
	}
	//free memory for allocated list
	while(pre_pointer_allo != NULL){

		free(pre_pointer_allo);
		pre_pointer_allo = pointer_allo;
		if(pointer_allo->next != NULL){
			pointer_allo = pointer_allo->next;
		}
	}
}

