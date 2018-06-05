#include <ijvm.h>

int init_ijvm(char *binary_file)
{
  // Implement loading of binary here FOR MODULE 1
  char *binary = binary_file;
  FILE *fp;
  char buffer[128];
  fp = fopen(binary, "rb");
  fread(buffer, sizeof(char), 128, fp);
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
