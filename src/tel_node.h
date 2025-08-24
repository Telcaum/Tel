#pragma once

#include "tel_lexer.h"
#include "tel_value.h"


typedef enum {
  TEL_NT_NUMBER,
  TEL_NT_PREFIX,
  TEL_NT_INFIX,
} Tel_NodeType;


typedef struct Tel_Node Tel_Node;

typedef union {
  struct {
    Tel_Number value;
  } number;
  struct {
    Tel_Node *arg;
  } prefix;
  struct {
    Tel_Node *lhs;
    Tel_Node *rhs;
  } infix;
} Tel_NodeAs;

struct Tel_Node {
  Tel_NodeType type;
  Tel_Token token;
  Tel_NodeAs as;
};


Tel_Node *Tel_newNode(Tel_NodeType type, Tel_Token token, Tel_NodeAs as);
