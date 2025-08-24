#include <stddef.h>
#include <stdio.h>

#include "tel.h"
#include "tel_debug.h"
#include "tel_error.h"
#include "tel_lexer.h"
#include "tel_node.h"
#include "tel_parser.h"
#include "tel_vm.h"


Tel_ExecutionResult Tel_run(char const *source) {
  Tel_Lexer lexer;
  Tel_initLexer(&lexer, source);

  Tel_ErrorLogger error_logger;
  Tel_initErrorLogger(&error_logger, source);
  Tel_Parser parser;
  Tel_initParser(&parser, &lexer, &error_logger);

  for (;;) {
    Tel_Node *node = Tel_buildTreeBranch(&parser);
    if (node == NULL) {
      break;
    }

    Tel_printNode(node, source);
    puts("");
  }

  Tel_VM vm;
  Tel_initVM(&vm);

  Tel_freeVM(&vm);

  return TEL_EXEC_OK;
}
