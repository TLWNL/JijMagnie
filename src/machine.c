#include <ijvm.h>
#include <stdlib.h>
#include <stdio.h>

static uint32_t swap_uint32(uint32_t num)
{
	return ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
  // Implement loading of binary here FOR MODULE 1
  
	FILE *fp;
	//uint32_t size;
	long filelength;							// Change this to filesize
	uint32_t *buffer;				
	int amount_of_cycles;						// Amount of cycles needed to run through the buffer

	fp = fopen(binary_file, "rb");				// Opens the file in binary mode 
	
	//Put in an if statement here that returns an error if the magic number is not 1deadfad

	// Get the file size
	fseek(fp, 0 , SEEK_END);					// Jump to the end of the file
	filelength = ftell(fp);						// Get the current byte offset
	printf("The total size of the binary file = %ld bytes\n", filelength);
	rewind(fp);									// Jump back to the beginning of the file 

	// Allocate memory for the entire file 
	buffer = (uint32_t*) malloc(filelength+1);
	//Add error checking here

	// Copy a word into the buffer 
	fread(buffer,sizeof(uint32_t), filelength, fp);

	fclose(fp);

	amount_of_cycles = filelength/4;

	// Print the buffered data 
	for (int i = 0; i < amount_of_cycles+1; i++)
	{
		buffer[i] = swap_uint32(buffer[i]);
		printf("Current buffer item: %x \n", buffer[i]);
	}
	
	free(buffer);
	return 0;	
}

void destroy_ijvm()
{
  // Reset IJVM state FOR MODULE 1
}

void run()
{
  // Step while you can FOR MODULE 1
}

void set_input(FILE *fp)
{
  // TODO: implement me 
}

void set_output(FILE *fp)
{
  // TODO: implement me
}
