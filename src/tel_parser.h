#pragma once

#include <stdbool.h>

#include "tel_error.h"
#include "tel_lexer.h"
#include "tel_node.h"


typedef struct {
  Tel_ErrorLogger *error_logger;
  bool is_panicking;
  Tel_Lexer *lexer;
  Tel_Token previous_token;
  Tel_Token current_token;
} Tel_Parser;


void Tel_initParser(Tel_Parser *self, Tel_Lexer *lexer, Tel_ErrorLogger *error_logger);

// returns NULL at EOF
Tel_Node *Tel_buildTreeBranch(Tel_Parser *self);
