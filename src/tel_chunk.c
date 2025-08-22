#include "tel_chunk.h"


void Tel_initChunk(Tel_Chunk *chunk) {
  TEL_LIST_INIT(&chunk->code, Tel_Inst);
  TEL_LIST_INIT(&chunk->lines, _Tel_InstructionLine);
  TEL_INIT_VALUE_LIST(&chunk->constants);
}


void Tel_freeChunk(Tel_Chunk *chunk) {
  TEL_LIST_FREE(&chunk->code);
  TEL_LIST_FREE(&chunk->lines);
  TEL_FREE_VALUE_LIST(&chunk->constants);
}


void Tel_writeInst(Tel_Chunk *chunk, Tel_Inst byte, int line) {
  TEL_LIST_PUSH(&chunk->code, byte, Tel_Inst);
  if (chunk->lines.length == 0 || chunk->lines.data[chunk->lines.length - 1].line != line) {
    _Tel_InstructionLine l = {};
    l.line = line;
    l.inst_index = chunk->code.length - 1;
    TEL_LIST_PUSH(&chunk->lines, l, _Tel_InstructionLine);
  }
}


int Tel_addConstant(Tel_Chunk *chunk, Tel_Value value) {
  TEL_WRITE_VALUE(&chunk->constants, value);
  return chunk->constants.length - 1;
}


void Tel_writeConstant(Tel_Chunk *chunk, Tel_Value value, Tel_Byte a, int line) {
  int index = Tel_addConstant(chunk, value);

  Tel_writeInst(chunk, TEL_ABB_INST(TEL_OP_LOAD_CONST, a, index), line);
}


int Tel_getLine(Tel_Chunk const *chunk, int instruction_index) {
  if (instruction_index < 0) return 0;

  for (int i = 0; i < chunk->lines.length - 1; i++) {
    _Tel_InstructionLine l = chunk->lines.data[i];
    _Tel_InstructionLine next_l = chunk->lines.data[i + 1];

    if (instruction_index >= l.inst_index && instruction_index < next_l.inst_index) return l.line;
  }

  return chunk->lines.data[chunk->lines.length - 1].line;
}
