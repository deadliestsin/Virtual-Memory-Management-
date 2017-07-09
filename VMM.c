#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
//declare the two tables
int pageTableNumbers[256][2];  
int physicalMemory[256][256]; 
//declare the counter variables  
int pageFaults = 0;      
int emptyFrame = 0;
int translatedAddresses = 0;
//declare the buffers for reading the files         
char address[10];
signed char buffer[256];
//declare the file pointers
FILE    *address_file;
FILE    *backing_store;

int ComputePhysicalAddress(int pageNumber);

int main(int argc, char *argv[]){
    //check if the use inputed the text name
	if (argc != 2) {
        fprintf(stderr,"please input file name: ./a.out [example.txt]\n");
        return -1;
    }
	//open the backing store and the file
    backing_store = fopen("BACKING_STORE.bin", "rb");
    address_file = fopen(argv[1], "r");				 
   	//loop through the addresses and calculate the page number and offset				
    while ( fgets(address, 10, address_file) != NULL){
		int pageNumber = ((atoi(address) & 0xFFFF)>>8);	 
		int offset = (atoi(address) & 0xFF);
		//compute the frame number using the page table		
        int frameNumber = ComputePhysicalAddress(pageNumber);
		//once the frame number is retrieved check the value in physical memory
		int value = physicalMemory[frameNumber][offset];
		//print the virtual and physical address		
		printf("%dVirtual address: %d Physical address: %d Value: %d\n",translatedAddresses, atoi(address), (frameNumber << 8) | offset, value);
        translatedAddresses++;
    }
    
	//output the stats on the page faults
    printf("Number of translated addresses = %d\n", translatedAddresses);
    printf("Page Faults = %d\n", pageFaults);
    printf("Page Fault Rate = %.3f\n", pageFaults / (double)translatedAddresses);
	//close the address and backing store files
	fclose(address_file);
    fclose(backing_store);
    return 0;
}
int ComputePhysicalAddress(int pageNumber){
    
	int frameNumber = -1; 
    int i;
	//check the table if the page exists
    for(i = 0; i <256; i++){
		if(pageTableNumbers[i][0] == pageNumber){
			//if the page exists collect its frame number and return 			
            frameNumber = pageTableNumbers[i][1]; 
        }
    }
	//else access the backing store
    if(frameNumber == -1){                
		int i;
		//seek the backing store
		if (fseek(backing_store, pageNumber * 256, SEEK_SET) != 0 || fread(buffer, sizeof(signed char), 256, backing_store) == 0 ) {
			fprintf(stderr, "Error seeking in backing store\n");
		}
		//store it in the available page frame
		for(i = 0; i < 256; i++){
			physicalMemory[emptyFrame][i] = buffer[i];
		}
		//then update the page table
		pageTableNumbers[emptyFrame][0] = pageNumber;
		pageTableNumbers[emptyFrame][1] = emptyFrame;
		//update the empty frame and page faults
		emptyFrame++;         
        pageFaults++;
		//calculate the frame number and return
        frameNumber = emptyFrame - 1; 
    }
	return frameNumber;	
}









