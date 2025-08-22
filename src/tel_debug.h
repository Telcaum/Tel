#pragma once

#include "tel_chunk.h"
#include "tel_lexer.h"


#define TEL_DEBUG_TRACE_EXECUTION
#define TEL_DEBUG_VM_BOUNDS_CHECK


void Tel_printToken(Tel_Token token, char const *source);

void printInstruction(Tel_Chunk const *chunk, int index);
void Tel_printChunk(Tel_Chunk const *chunk, char const *name);
