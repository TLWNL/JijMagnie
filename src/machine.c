#include <ijvm.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

uint32_t *buffer;
long filelength;
int amount_of_cycles;


static uint32_t swap_uint32(uint32_t num)
{
	return ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
  // Implement loading of binary here FOR MODULE 1
  
	FILE *fp;			

	fp = fopen(binary_file, "rb");				// Opens the file in binary mode 

	// Get the file size
	fseek(fp, 0 , SEEK_END);					// Jump to the end of the file
	filelength = ftell(fp);						// Get the current byte offset
	amount_of_cycles = filelength/4;
	printf("The total size of the binary file = %ld bytes\n", filelength);
	rewind(fp);									// Jump back to the beginning of the file 

	// Allocate memory for the entire file 
	buffer = (uint32_t*) malloc(filelength+1);	// Allocate memory for the file + \0 
	//Add error checking here

	// Copy a word into the buffer 
	fread(buffer,sizeof(uint32_t), filelength, fp);
	fclose(fp);

	for (int i=0; i<amount_of_cycles+1; i++)
	{
		buffer[i] = swap_uint32(buffer[i]);
	}
	// Error checking for the case that the magic number is not 1deadfad
	if (buffer[0] != 0x1deadfad)
	{
		printf("%02x", buffer[0]);
		fprintf(stderr, "Binary file not supported, only .ijvm files supported. Program aborted.\n");
		return -1;
	}
	return 0;	
}

void destroy_ijvm()
{
  // Reset IJVM state FOR MODULE 1
}

void run()
{
	//get_text();
	step();
}


bool step()
{
	uint32_t byte1;
	uint32_t byte2;
	uint32_t byte3;
	uint32_t byte4;
	int size_const_in_bytes;
	int size_text_in_bytes;
	int program_counter = 0;

	//Step (Run though one by one) though all the instructions
	// A good starting point is: loop through the text selection of the binary.
	// Print the name of every instruction you have encountered
	// If you encounter a byte that is not an instruction, skip that byte.
	// Don't worry about arguments that have the same value as an instuction, just
	// print the name of the corresponding instruction.
	// Read one byte, determine instruction. ect. ect.
	// Also now is the perfect time to implement a program counter mechanism.

	// Buffered data is in words of 2 bytes.
	// Convert to string, then split it into two vars

	// binary file = <32-bit magic number>
	// 2 blocks 1) Constants, 2) TEXT.
	// block = <32-bit origin> <32-bit byte size> <data>

	// Size is in bytes, 2 bytes per word, so move forward int version of size / 2.
	
	// Buffer [0] and Buffer 1 can be discarded
	
	for(int i = 2; i < amount_of_cycles+1; i++)
	{
		
		printf("Current buffer item: %02x \n", buffer[i]);
		byte1 = (buffer[i]>>24) & 0xff;
		printf("Byte 1: %02x\n", byte1);
		byte2 = (buffer[i]>> 16) & 0xff;
		printf("Byte 2: %02x\n", byte2);
		byte3 = (buffer[i]>> 8) & 0xff;
		printf("Byte 3: %02x\n", byte3);
		byte4 = (buffer[i] & 0xff);
		printf("Byte 4: %02x\n", byte4);

		switch(byte1)
		{
			case '0x10'		// Takes byte arg
			{
				printf("BIPUSH");
			}
			case '0x59'
			{
				printf("DUP");
			}
			case '0xfe'
			{
				printf("ERR");
			}
			case '0xa7'
			{
				printf("GOTO")			// Takes short arg 
			}
			case '0xff'
			{
				printf("HALT");
			}
			case '0x60'
			{
				printf("IADD");
			}
			case '0x7e'
			{
				printf("IAND");
			}
			case '0x99'
			{
				printf("IFEQ");			// Takes short arg 
			}
			case '0x9b'
			{
				printf("IFLT");			// Takes short arg 
			}
			case '0x9f'
			{
				printf("IF_ICMPEQ");	// Takes short arg 
			}
			case '0x84'
			{
				printf("IINC");			// Takes byte byte arg 
			}
			case '0x15'
			{
				printf("ILOAD");		// Takes byte arg
			}
			case '0xfc'
			{
				printf("IN");
			}
			case '0xb6'
			{
				printf("INVOKEVIRTUAL");	// Takes short arg
			}
			case '0xb0'
			{
				printf("IOR");
			}
			case '0xac'
			{
				printf("IRETURN");
			}
			case '0x36'
			{
				printf("ISTORE");			// Takes byte arg
			}
			case '0x64'
			{
				printf("ISUB");
			}
			case '0x13'
			{
				printf("LDC_W");			// Takes short arg
			}
			case '0x00'
			{
				printf("NOP");
			}
			case '0xfd'
			{
				printf("OUT");
			}
			case '0x57'
			{
				printf("POP");
			}
			case '0x5f'
			{
				printf("SWAP");
			}
			case '0xc4'
			{
				printf("WIDE");
			}

		}
	}
	return 0;
}

byte_t *get_text()
{

	// Print the buffered data 
	for (int i = 0; i < amount_of_cycles+1; i++)
	{
		printf("Current buffer item: %02x \n", buffer[i]);
	}
	return 0;
}

void set_input(FILE *fp)
{
  // TODO: implement me 
}

void set_output(FILE *fp)
{
  // TODO: implement me
}
