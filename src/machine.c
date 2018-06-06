#include <ijvm.h>
#include <stdlib.h>
#include <stdio.h>

static uint32_t swap_uint32(uint32_t num)
{
	return ((num>>24)&0x0ff) | ((num<<8)&0x0ff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
  // Implement loading of binary here FOR MODULE 1
	FILE *fp;
	//uint32_t size;
	long filelength;							// Change this to filesize
	char *buffer;
	uint32_t result;						

	fp = fopen(binary_file, "rb");				// Opens the file in binary mode 
	
	//Put in an if statement here that returns an error if the magic number is not 1deadfad

	// Get the file size
	fseek(fp, 0 , SEEK_END);					// Jump to the end of the file
	filelength = ftell(fp);						// Get the current byte offset
	printf("The total size of the binary file = %ld bytes\n", filelength);
	rewind(fp);									// Jump back to the beginning of the file 

	// Reads the 32-bit magic number
	/*
	fread(&size, filelength, 1, fp);
	size = swap_uint32(size);
	*/

	// Allocate memory for the entire file 
	buffer = (char*) malloc(filelength);		// Why (char*) malloc???
	//Add error checking here

	// Copy the file into the buffer 
	result = fread(buffer,filelength, 1, fp);
	result = swap_uint32(result);

	fclose(fp);

	// Print the buffered data 
	printf("Total File in Hex:\n");
	printf("%u", result);
	/*for (int i = 0; i < filelength; i++)
	{
		printf("%02x", buffer[i]);
	}*/
	free(buffer);
	return 0;	
	//printf("Magic Number in Hex: %02x", size);
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
