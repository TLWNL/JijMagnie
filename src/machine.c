#include <ijvm.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int program_counter = 0;
byte_t current_byte;
bool byte_argument;
bool short_argument;

struct CURRENT{
	word_t magic_number;
	word_t constant_pool_origin;
	word_t constant_pool_size;
	word_t *constant_data;
	word_t text_origin;
	word_t text_size;
	byte_t *text_data;
};
struct CURRENT DATA;


static uint32_t swap_uint32(uint32_t num)
{
	return ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
	FILE *fp;			

	fp = fopen(binary_file, "rb");				// Opens the file in binary mode 

	// Copy magic number into the struct 
	fread(&DATA.magic_number,sizeof(word_t), 1, fp);
	//Change endian
	DATA.magic_number = swap_uint32(DATA.magic_number);
		// Error checking for the case that the magic number is not 1deadfad
	if (DATA.magic_number != 0x1deadfad)
	{
		fprintf(stderr, "Binary file not supported, only .ijvm files are supported. Program aborted.\n");
		return -1;
	}

	// Copy constant pool origin into the struct
	fread(&DATA.constant_pool_origin,sizeof(word_t), 1, fp);
	DATA.constant_pool_origin = swap_uint32(DATA.constant_pool_origin);

	// Copy constant pool size into the struct
	fread(&DATA.constant_pool_size, sizeof(word_t), 1, fp);
	DATA.constant_pool_size = swap_uint32(DATA.constant_pool_size);

	// Allocate memory for the constant pool data
	DATA.constant_data = malloc(DATA.constant_pool_size);
	// Read constant pool into the mallocated space
	fread(DATA.constant_data, sizeof(word_t), DATA.constant_pool_size / 4, fp);	// Constant pool size is in bytes, constant data is in words, so divide by 4
	// Change endian of the constant pool data
	for(unsigned int i=0; i<DATA.constant_pool_size / 4; i++)
	{
		DATA.constant_data[i] = swap_uint32(DATA.constant_data[i]);
	}

	// Read text origin into the struct
	fread(&DATA.text_origin, sizeof(word_t), 1, fp);
	DATA.text_origin = swap_uint32(DATA.text_origin);

	// Read text size into the struct
	fread(&DATA.text_size, sizeof(word_t), 1, fp);
	DATA.text_size = swap_uint32(DATA.text_size);

	// Allocate memory for the text data
	DATA.text_data = malloc(DATA.text_size);
	// Read the text data in bytes
	fread(DATA.text_data, sizeof(byte_t), DATA.text_size, fp);

	return 0;	
}

void destroy_ijvm()
{
	free(DATA.text_data);
	free(DATA.constant_data);
 }

void run()
{

	byte_t stored_byte_argument;
	byte_t stored_short_argument[2];
	int arguments_passed = 0;

	for (unsigned int i=0; i<DATA.text_size; i++)
	{
		current_byte = DATA.text_data[i];

		if(byte_argument == true)
		{
			stored_byte_argument = current_byte;
			byte_argument = false;
			continue;
		}
		if(short_argument == true)
		{
			if (arguments_passed == 0)
			{
				stored_short_argument[0] = current_byte;
				arguments_passed += 1;
				continue;
			}
			if (arguments_passed == 1)
			{
				stored_short_argument[1] = current_byte;
				arguments_passed = 0;
				short_argument = false;
				continue;
			}
		}
		step();
	}
}

bool step()
{
	switch(current_byte)
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
			break;
		}
		case 0xff:
		{
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
			short_argument = true;
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
			break;
		}
	}

	//get_program_counter();
	return 0;
}

byte_t *get_text()
{
	return DATA.text_data;
}

int get_program_counter()
{
	//printf("The current program counter is: %d \n", program_counter);
	return program_counter;
}

byte_t get_instruction()
{
	//printf("The current instruction is: %02x \n", current_byte);
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

int text_size()
{
	int text_size_int = DATA.text_size;
	return text_size_int;
}