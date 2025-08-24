#include <assert.h>
#include <stddef.h>

#include "tel_compiler.h"
#include "tel_chunk.h"
#include "tel_lexer.h"
#include "tel_node.h"
#include "tel_value.h"


static bool compileNumber(Tel_Chunk *chunk, Tel_Node *node, Tel_Byte destination_slot, Tel_Byte next_free_stack_slot) {
  Tel_writeConstant(chunk, TEL_NUMBER(node->as.number.value), destination_slot, node->token.line);
  return true;
}


static Tel_OpCode getInfixOperatorOpCode(Tel_TokenType token_type) {
  switch (token_type) {
    case TEL_TT_PLUS: return TEL_OP_ADD;
    case TEL_TT_MINUS: return TEL_OP_SUB;
    case TEL_TT_STAR: return TEL_OP_MUL;
    case TEL_TT_SLASH: return TEL_OP_DIV;

    default:
      assert(0);
  }
}


static bool compileInfix(Tel_Chunk *chunk, Tel_Node *node, Tel_Byte destination_slot, Tel_Byte next_free_stack_slot) {
  Tel_OpCode operator_opcode = getInfixOperatorOpCode(node->token.type);

  bool ok = Tel_compileExpression(
    chunk, node->as.infix.lhs,
    next_free_stack_slot, next_free_stack_slot
  );
  ok = ok && Tel_compileExpression(
    chunk, node->as.infix.rhs,
    next_free_stack_slot + 1, next_free_stack_slot + 1
  );

  Tel_writeInst(
    chunk,
    TEL_ABC_INST(operator_opcode, destination_slot, next_free_stack_slot, next_free_stack_slot + 1),
    node->token.line
  );

  return ok;
}


static Tel_OpCode getPrefixOperatorOpCode(Tel_TokenType token_type) {
  switch (token_type) {
    case TEL_TT_MINUS: return TEL_OP_NEGATE;

    default:
      assert(0);
  }
}


static bool compilePrefix(Tel_Chunk *chunk, Tel_Node *node, Tel_Byte destination_slot, Tel_Byte next_free_stack_slot) {
  Tel_OpCode operator_opcode = getPrefixOperatorOpCode(node->token.type);

  bool ok = Tel_compileExpression(chunk, node->as.prefix.arg, next_free_stack_slot, next_free_stack_slot);

  Tel_writeInst(chunk, TEL_AB__INST(operator_opcode, destination_slot, next_free_stack_slot), node->token.line);

  return ok;
}


bool Tel_compileExpression(Tel_Chunk *chunk, Tel_Node *node, Tel_Byte destination_slot, Tel_Byte next_free_stack_slot) {
  if (node == NULL) {
    return false;
  }

  switch (node->type) {
    case TEL_NT_NUMBER:
      return compileNumber(chunk, node, destination_slot, next_free_stack_slot);

    case TEL_NT_INFIX:
      return compileInfix(chunk, node, destination_slot, next_free_stack_slot);

    case TEL_NT_PREFIX:
      return compilePrefix(chunk, node, destination_slot, next_free_stack_slot);
  }

  assert(0);
}
