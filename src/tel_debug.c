#include <stdio.h>

#include "tel_debug.h"


void Tel_printToken(Tel_Token token, char const *source) {
  printf(
    "Token(type = %s, content = '%.*s', line = %d, indent = %d)",
    Tel_getTokenTypeName(token.type), token.length, &source[token.start], token.line, token.indent
  );
}


void Tel_printNode(Tel_Node *node, char const *source) {
  if (node == NULL) {
    printf("?");
    return;
  }

  switch (node->type) {
    case TEL_NT_NUMBER:
      printf("%g", node->as.number.value);
      break;

    case TEL_NT_PREFIX:
      printf("(%.*s ", node->token.length, &source[node->token.start]);
      Tel_printNode(node->as.prefix.arg, source);
      printf(")");
      break;

    case TEL_NT_INFIX:
      printf("(%.*s ", node->token.length, &source[node->token.start]);
      Tel_printNode(node->as.infix.lhs, source);
      printf(" ");
      Tel_printNode(node->as.infix.rhs, source);
      printf(")");
      break;
  }
}


static void simpleInstruction(char const *name) {
  printf("%s\n", name);
}


static void constantInstruction(Tel_Chunk const *chunk, Tel_Inst inst) {
  int a = TEL_GET_A__(inst);
  int bb = TEL_GET__BB(inst);
  printf("%-16s %03d %07d (%g)\n", "LOAD_CONST", a, bb, chunk->constants.data[bb].number);
}


static void AB_Instruction(char const *name, Tel_Inst inst) {
  int a = TEL_GET_A__(inst);
  int b = TEL_GET__B_(inst);
  printf("%-16s %03d %03d\n", name, a, b);
}


static void ABCInstruction(char const *name, Tel_Inst inst) {
  int a = TEL_GET_A__(inst);
  int b = TEL_GET__B_(inst);
  int c = TEL_GET___C(inst);
  printf("%-16s %03d %03d %03d\n", name, a, b, c);
}


void printInstruction(Tel_Chunk const *chunk, int index) {
  printf("%04d ", index);

  int line = Tel_getLine(chunk, index);
  if (index > 0 && Tel_getLine(chunk, index - 1) == line) {
    printf("   | ");
  }
  else {
    printf("%4d ", line);
  }

  Tel_Inst inst = chunk->code.data[index];

  Tel_OpCode op_code = TEL_GET_INST_OP_CODE(inst);

  switch (op_code) {
    #define AB__INST(name) case TEL_OP_ ## name: AB_Instruction(#name, inst); return;
    #define ABC_INST(name) case TEL_OP_ ## name: ABCInstruction(#name, inst); return;
    case TEL_OP_RETURN:
      simpleInstruction("RETURN");
      return;
    case TEL_OP_LOAD_CONST:
      constantInstruction(chunk, inst);
      return;
    AB__INST(NEGATE);
    ABC_INST(ADD);
    ABC_INST(SUB);
    ABC_INST(MUL);
    ABC_INST(DIV);
    AB__INST(MOVE);

    #undef AB__INST
    #undef ABC_INST
  }

  //printf("%-16s %03d %03d %03d\n", "???", TEL_GET_A__(inst), TEL_GET__B_(inst), TEL_GET___C(inst));
  ABCInstruction("???", inst);
}


void Tel_printChunk(Tel_Chunk const *chunk, char const *name) {
  printf("== %s ==\n", name);

  for (int i = 0; i < chunk->code.length; i++) {
    printInstruction(chunk, i);
  }
}
