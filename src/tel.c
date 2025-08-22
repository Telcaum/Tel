#include "tel.h"
#include "tel_lexer.h"
#include "tel_vm.h"


Tel_ExecutionResult Tel_run(char const *source) {
  Tel_Lexer lexer = {};
  Tel_initLexer(&lexer, source);

  Tel_VM vm = {};
  Tel_initVM(&vm);

  Tel_freeVM(&vm);

  return TEL_EXEC_OK;
}
