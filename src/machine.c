#include <ijvm.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <customincludes.h>
#include <string.h>

int program_counter = 0;
byte_t current_byte;
bool byte_argument;
bool short_argument;
byte_t current_op;
byte_t stored_short_argument[4];

// Create a linked list
struct node
{
	word_t data;
	struct node *next;
};
typedef struct node node;

node *top;

// Initialize the stack
void init()
{
	top = NULL;
}

/* Push operation:
** 1. Make a new node.
** 2. Give the 'data' of the new node its value.
** 3. Point the 'next' of the new node to the top of the stack.
** 4. Make the 'top' pointer point to this new node.
*/
void push(word_t value)
{
	printf("Value to push = %02x\n", value);
	// What should I assert here? It is a dynamic stack so asserting if the top < STACK_SIZE - 1 is pretty useless...
	node *tmp;
	tmp = malloc(sizeof(node));

	tmp->data = value;

	tmp->next = top;

	top = tmp;

	printf("Push successful!\n");

}


/* Pop the top element from the stack and return it 
** 1. Make a temp node
** 2. Point this temp node to the top of the stack
** 3. Store the value of 'data' of this temp node in a var
** 4. Point the 'top' pointer to the node next to the current top node.
** 5. Delete the temp node using free.
** 6. Return the value stored in the var.
*/
word_t pop()
{
	//assert(top > 0);
	node *tmp;
	word_t n;
	tmp = top;
	n = tmp->data;
	top = top->next;
	free(tmp);
	printf("Pop successful!\n");
	return n;
}

/**
 * This function should return the word at the top of the stack of the current
 * frame, interpreted as a signed integer.
 **/
word_t tos()
{
	//assert(top > 0);
	printf("The top element of the stack is: %d", top->data);
	return top->data;
}

bool isEmpty()
{
	if(top==NULL)
		return true;
	else
		return false;
}

// Return the size of the stack as an in
/* WIP
int stack_size(struct node* head)
{
	int count = 0;
	struct node* tmp = head;
	while (tmp != NULL)
	{
		count ++;
		tmp = tmp->next;
	}
	return count;
}

*/



struct CURRENT{
	word_t magic_number;
	word_t constant_pool_origin;
	word_t constant_pool_size;
	word_t *constant_data;
	word_t text_origin;
	word_t text_size;
	byte_t *text_data;
};
struct CURRENT FILEDATA;


static uint32_t swap_uint32(uint32_t num)
{
	return ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int init_ijvm(char *binary_file)
{
	FILE *fp;			

	fp = fopen(binary_file, "rb");				// Opens the file in binary mode 

	// Copy magic number into the struct 
	fread(&FILEDATA.magic_number,sizeof(word_t), 1, fp);
	//Change endian
	FILEDATA.magic_number = swap_uint32(FILEDATA.magic_number);
		// Error checking for the case that the magic number is not 1deadfad
	if (FILEDATA.magic_number != 0x1deadfad)
	{
		fprintf(stderr, "Binary file not supported, only .ijvm files are supported. Program aborted.\n");
		return -1;
	}

	// Copy constant pool origin into the struct
	fread(&FILEDATA.constant_pool_origin,sizeof(word_t), 1, fp);
	FILEDATA.constant_pool_origin = swap_uint32(FILEDATA.constant_pool_origin);

	// Copy constant pool size into the struct
	fread(&FILEDATA.constant_pool_size, sizeof(word_t), 1, fp);
	FILEDATA.constant_pool_size = swap_uint32(FILEDATA.constant_pool_size);

	// Allocate memory for the constant pool data
	FILEDATA.constant_data = malloc(FILEDATA.constant_pool_size);
	// Read constant pool into the mallocated space
	fread(FILEDATA.constant_data, sizeof(word_t), FILEDATA.constant_pool_size / 4, fp);	// Constant pool size is in bytes, constant data is in words, so divide by 4
	// Change endian of the constant pool data
	for(unsigned int i=0; i<FILEDATA.constant_pool_size / 4; i++)
	{
		FILEDATA.constant_data[i] = swap_uint32(FILEDATA.constant_data[i]);
	}

	// Read text origin into the struct
	fread(&FILEDATA.text_origin, sizeof(word_t), 1, fp);
	FILEDATA.text_origin = swap_uint32(FILEDATA.text_origin);

	// Read text size into the struct
	fread(&FILEDATA.text_size, sizeof(word_t), 1, fp);
	FILEDATA.text_size = swap_uint32(FILEDATA.text_size);

	// Allocate memory for the text data
	FILEDATA.text_data = malloc(FILEDATA.text_size);
	// Read the text data in bytes
	fread(FILEDATA.text_data, sizeof(byte_t), FILEDATA.text_size, fp);

	return 0;	
}

void destroy_ijvm()
{
	free(FILEDATA.text_data);
	free(FILEDATA.constant_data);
 }

void run()
{
	int arguments_passed = 0;
	word_t stored_word_argument;

	for (unsigned int i=0; i<FILEDATA.text_size; i++)
	{
		current_byte = FILEDATA.text_data[i];

		if(byte_argument == true)
		{
			stored_word_argument = (word_t) current_byte;

			// Maybe make this a reusable function.
			//memcpy(stored_word_argument, stored_byte_argument, sizeof(byte_argument));

			//hex_to_string(stored_word_argument);
			//printf("The word in hex is: %02x\n", stored_word_argument[0]);
			
			switch(current_op)
			{
				case 0x10:
				{
					push(stored_word_argument);
					break;
				}
			}
			byte_argument = false;
			continue;
		}
		if(short_argument == true)
		{
			if (arguments_passed == 0)
			{
				stored_short_argument[2] = current_byte;
				arguments_passed += 1;
				continue;
			}
			if (arguments_passed == 1)
			{
				stored_short_argument[3] = current_byte;
				hex_to_string(stored_short_argument);
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
			current_op = 0x10;
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
			program_counter = program_counter + 1;
			break;
		}
		case 0x60:
		{
			word_t firstVal;
			word_t secondVal;
			int sum;

			printf("IADD\n");
			// Pop () Pop (), convert to int, sum, convert back to word_t and push.
			firstVal = pop();
			secondVal = pop();

			firstVal = firstVal & 0xFF;

			printf("First val = %02x", firstVal);
			printf("Second val = %02x", secondVal);

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
	return FILEDATA.text_data;
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
	int text_size_int = FILEDATA.text_size;
	return text_size_int;
}

// Convert a hex byte argument to a 32-bit integer 
/*void convert_to__int(word_t bytes)
{
	/* Big endianness
	int result = ((bytes[0] & 0xFF) << 24) | ((bytes[1] & 0xFF) << 16  | ((bytes[2] & 0xFF) << 8) | (bytes[3] & 0xFF);
	

	// Little endianness
	int result = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
}*/

void hex_to_string(byte_t byte_array[4])
{
	for (int i=0;i<4;i++)
	{
		printf("%02x ", byte_array[i]);
	}
}
