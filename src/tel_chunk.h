#pragma once

#include <stdint.h>

#include "tel_memory.h"
#include "tel_value.h"


// R(X): the Xth register
// C(X): the Xth constant
// X   : a 1-byte operand
// X.X : a 2-byte operand
typedef enum {
  #define _TEL_OP(name) TEL_OP_ ## name,
  #include "tel_opcodes.h"
  #undef _TEL_OP
} Tel_OpCode;


typedef uint8_t Tel_Byte;
typedef uint32_t Tel_Inst;

typedef TEL_LIST(Tel_Inst) Tel_InstList;


typedef struct {
  int inst_index;
  int line;
} _Tel_InstructionLine;

typedef TEL_LIST(_Tel_InstructionLine) _Tel_InstructionLineList;


typedef struct {
  Tel_InstList code;
  _Tel_InstructionLineList lines;
  Tel_ValueList constants;
} Tel_Chunk;


#define TEL_GET_INST_OP_CODE(inst) (Tel_OpCode)((inst) & 0xff)
#define TEL_GET_A__(inst) (Tel_Byte)(((inst) >> 8) & 0xff)
#define TEL_GET__B_(inst) (Tel_Byte)(((inst) >> 16) & 0xff)
#define TEL_GET___C(inst) (Tel_Byte)(((inst) >> 24) & 0xff)
#define TEL_GET__BB(inst) (Tel_Inst)(((inst) >> 16) & 0xffff)

#define TEL_____INST(inst) \
  ((Tel_Inst)((inst) & 0xff))
#define TEL_ABB_INST(inst, a, bb) \
  ((Tel_Inst)((inst) & 0xff) | (Tel_Inst)(((a) & 0xff) << 8) | (Tel_Inst)(((bb) & 0xffff) << 16))
#define TEL_ABC_INST(inst, a, b, c) \
  ((Tel_Inst)((inst) & 0xff) | (Tel_Inst)(((a) & 0xff) << 8) | (Tel_Inst)(((b) & 0xff) << 16) | (Tel_Inst)(((c) & 0xff) << 24))
#define TEL_AB__INST(inst, a, b) \
  ((Tel_Inst)((inst) & 0xff) | (Tel_Inst)(((a) & 0xff) << 8) | (Tel_Inst)(((b) & 0xff) << 16))


void Tel_initChunk(Tel_Chunk *chunk);
void Tel_freeChunk(Tel_Chunk *chunk);

void Tel_writeInst(Tel_Chunk *chunk, Tel_Inst byte, int line);

int Tel_addConstant(Tel_Chunk *chunk, Tel_Value value);
void Tel_writeConstant(Tel_Chunk *chunk, Tel_Value value, Tel_Byte a, int line);

int Tel_getLine(Tel_Chunk const *chunk, int instruction_index);
