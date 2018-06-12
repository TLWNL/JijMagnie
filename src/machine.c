#include <ijvm.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

uint32_t *buffer;
uint32_t byte1;				// Check if you can make these local variables, STRUCT?! That may look kewl
uint32_t byte2;
uint32_t byte3;
uint32_t byte4;
int program_counter = 0;
uint32_t current_byte;
long filelength;
int amount_of_words;


static uint32_t swap_uint32(uint32_t num)
{
	return ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
	FILE *fp;			

	fp = fopen(binary_file, "rb");				// Opens the file in binary mode 
	// Get the file size
	fseek(fp, 0 , SEEK_END);					// Jump to the end of the file
	filelength = ftell(fp);						// Get the current byte offset
	amount_of_words = filelength/4;
	printf("The total size of the binary file = %ld bytes\n", filelength);
	rewind(fp);									// Jump back to the beginning of the file 

	// Allocate memory for the entire file 
	buffer = (uint32_t*) malloc(filelength+1);	// Allocate memory for the file + \0 
	//Add error checking here

	// Copy a word into the buffer 
	fread(buffer,sizeof(uint32_t), filelength, fp);
	fclose(fp);

	// Change endian
	for (int i=0; i<amount_of_words+1; i++)
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
  free(buffer);
  exit(0);
 }

void run()
{
	int constant_size;
	int j;

	// Run through the buffer word for word
	for(int i = 1; i < amount_of_words+1; i++)		// Do I read through an extra word here?
	{		
		byte1 = (buffer[i]>>24) & 0xff;
		byte2 = (buffer[i]>> 16) & 0xff;
		byte3 = (buffer[i]>> 8) & 0xff;
		byte4 = (buffer[i] & 0xff);
		
		if(i == 1)
		{
			printf("The origin of the constant pool is %02x\n", buffer[i]);
		}
		// Start reading the constants
		else if(i == 2)
		{
			constant_size = buffer[i];
			printf("The size of the constant pool in bytes is: %d \n", constant_size);
			uint32_t constants[constant_size];
			for (j = 2; j < constant_size+2; j++)
			{
				constants[j] = buffer[i];
			}
			i += constant_size/4;
		}
		else if(i == constant_size/4 + 3)			// This is the word directly after the constant pool: the origin of TEXT
		{
			printf("The origin of the TEXT pool is: %02x \n", buffer[i]);
		}
		else if(i == constant_size/4 + 4)			// This is the size of the text pool.
		{
			printf("The size of the TEXT pool is: %d \n", buffer[i]);
		}
		else
		{
			step();
		}
	}	
}

bool step()
{
	bool byte_argument = false;
	bool short_argument;

	for(int i=0; i<4; i++)
	{

		switch(i)
		{
			case 0:
			{
				current_byte = byte1;
				//printf("Current byte = %02x \n", current_byte);
				break;
			}
			case 1:
			{
				current_byte = byte2;
				//printf("Current byte = %02x \n", current_byte);
				break;
			}
			case 2:
			{
				current_byte = byte3;
				//printf("Current byte = %02x \n", current_byte);
				break;
			}
			case 3:
			{
				current_byte = byte4;
				//printf("Current byte = %02x \n", current_byte);
				break;
			}
		}

		if(byte_argument == true)
		{
			printf("Byte argument = %02x\n", current_byte);
			byte_argument = false;
			continue;
		}
		
		switch(current_byte)		// Check if increasing the i really works 
		{
			case 0x10: // Takes byte arg
			{
				printf("BIPUSH \n");
				program_counter = program_counter + 1;
				byte_argument = true;
				break;
			}
			case 0x59:
			{
				printf("DUP\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0xfe:
			{
				printf("ERR\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0xa7:
			{
				printf("GOTO\n");			// Takes short arg ( 2 bytes )
				program_counter = program_counter + 1;
				short_argument = true;
				i += 2;
				break;
			}
			case 0xff:
			{
				destroy_ijvm();
				printf("HALT\n");
				//program_counter = program_counter + 1;
				//break;
			}
			case 0x60:
			{
				printf("IADD\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0x7e:
			{
				printf("IAND\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0x99:
			{
				printf("IFEQ\n");			// Takes short arg 
				program_counter = program_counter + 1;
				short_argument = true;
				break;
			}
			case 0x9b:
			{
				printf("IFLT\n");			// Takes short arg 
				program_counter = program_counter + 1;
				short_argument = true;
				break;
			}
			case 0x9f:
			{
				printf("IF_ICMPEQ\n");	// Takes short arg 
				program_counter = program_counter + 1;
				break;
			}
			case 0x84:
			{
				printf("IINC\n");			// Takes byte byte arg 
				program_counter = program_counter + 1;
				short_argument = true;
				break;
			}
			case 0x15:
			{
				printf("ILOAD\n");		// Takes byte arg
				program_counter = program_counter + 1;
				byte_argument = true;
				break;
			}
			case 0xfc:
			{
				printf("IN\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0xb6:
			{
				printf("INVOKEVIRTUAL\n");	// Takes short arg
				program_counter = program_counter + 1;
				short_argument = true;
				break;
			}
			case 0xb0:
			{
				printf("IOR\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0xac:
			{
				printf("IRETURN\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0x36:
			{
				printf("ISTORE\n");			// Takes byte arg
				program_counter = program_counter + 1;
				byte_argument = true;
				break;
			}
			case 0x64:
			{
				printf("ISUB\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0x13:
			{
				printf("LDC_W\n");			// Takes short arg
				program_counter = program_counter + 1;
				short_argument = true;
				break;
			}
			case 0x00:
			{
				printf("NOP\n");
				//program_counter = program_counter + 1;			// Does the program counter get increased after 00?
				break;
			}
			case 0xfd:
			{
				printf("OUT\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0x57:
			{
				printf("POP\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0x5f:
			{
				printf("SWAP\n");
				program_counter = program_counter + 1;
				break;
			}
			case 0xc4:
			{
				printf("WIDE\n");
				program_counter = program_counter + 1;
				break;
			}
			// Add EOF case

			// Add \n case

			// Change default to somtething that makes sense
			default:
			{
				printf("This is the default message\n");
				break;
			}
		}
	}
	get_program_counter();
	return 0;
}

byte_t *get_text()
{

	// Print the buffered data 
	for (int i = 0; i < amount_of_words+1; i++)
	{
		printf("Current buffer item: %02x \n", buffer[i]);
	}
	return 0;
}

int get_program_counter()
{
	printf("The current program counter is: %d \n", program_counter);
	return program_counter;
}

byte_t get_instruction()
{
	printf("The current instruction is: %02x \n", current_byte);

	return current_byte;
}

void set_input(FILE *fp)
{
  // TODO: implement me 
}

void set_output(FILE *fp)
{
  // TODO: implement me
}