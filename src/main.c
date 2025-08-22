#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tel.h"


static char *readFile(char const *path) {
  FILE *file = fopen(path, "r");

  if (file == NULL) {
    fprintf(stderr, "Could not open file '%s'.\n", path);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *content = (char *)malloc(sizeof(char) * (size + 1));
  TEL_CHECK_OOM(content);

  size_t read_size = fread(content, sizeof(char), size, file);
  if (read_size < size) {
    fprintf(stderr, "Could not read file '%s'.\n", path);
    free(content);
    return NULL;
  }

  content[size] = '\0';
  fclose(file);

  return content;
}


int main(int argc, char const **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path>\n", argv[0]);
    return 1;
  }

  char *source = readFile(argv[1]);
  if (source == NULL) return 1;

  /* Tel_Chunk chunk = {};
  Tel_initChunk(&chunk);

  Tel_writeConstant(&chunk, TEL_NUMBER(1.2), 0, 123);
  Tel_writeConstant(&chunk, TEL_NUMBER(1.3), 1, 123);
  Tel_writeInst(&chunk, TEL_ABC_INST(TEL_OP_SUB, 0, 0, 1), 235);
  Tel_writeInst(&chunk, TEL_AB__INST(TEL_OP_MOVE, 3, 1), 456);
  Tel_writeInst(&chunk, TEL_____INST(TEL_OP_RETURN), 456);

  Tel_printChunk(&chunk, "test");
  puts("");

  Tel_VM vm = {};
  Tel_initVM(&vm);

  Tel_execute(&vm, &chunk);

  Tel_freeVM(&vm);
  Tel_freeChunk(&chunk); */

  int exit_code = Tel_run(source);

  free(source);

  return exit_code;
}
