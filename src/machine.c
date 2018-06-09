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
	for(int i = 2; i < amount_of_cycles+1; i++)
	{
		
		//printf("Current buffer item: %02x \n", buffer[i]);
		byte1 = (buffer[i]>>24) & 0xff;
		step();
		//printf("Byte 1: %02x\n", byte1);
		byte2 = (buffer[i]>> 16) & 0xff;
		step();
		//printf("Byte 2: %02x\n", byte2);
		byte3 = (buffer[i]>> 8) & 0xff;
		step();
		//printf("Byte 3: %02x\n", byte3);
		byte4 = (buffer[i] & 0xff);
		step();
		//printf("Byte 4: %02x\n", byte4);
	}	
}


bool step()
{
	uint32_t current_byte;
	
	for(int i=0; i<2; i++)
	{
		if (i == 0)
		{
			current_byte = byte1;
		}
		else
		{
			current_byte = byte3;
		}
		
		switch(current_byte)
		{
			case 0x10: // Takes byte arg
			{
				printf("BIPUSH\n");
				program_counter = program_counter + 1;
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
				printf("GOTO\n");			// Takes short arg 
				program_counter = program_counter + 1;
				break;
			}
			case 0xff:
			{
				printf("HALT\n");
				program_counter = program_counter + 1;
				break;
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
				break;
			}
			case 0x9b:
			{
				printf("IFLT\n");			// Takes short arg 
				program_counter = program_counter + 1;
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
				break;
			}
			case 0x15:
			{
				printf("ILOAD\n");		// Takes byte arg
				program_counter = program_counter + 1;
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
				break;
			}
			case 0x00:
			{
				printf("NOP\n");
				program_counter = program_counter + 1;			// Does the program counter get increased after 00?
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
				program_counter = program_counter + 1;
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
	for (int i = 0; i < amount_of_cycles+1; i++)
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
}

void set_input(FILE *fp)
{
  // TODO: implement me 
}

void set_output(FILE *fp)
{
  // TODO: implement me
}
