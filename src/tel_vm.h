#pragma once

#include "tel_chunk.h"
#include "tel_value.h"


#define TEL_COMPUTED_GOTOS 1

#define TEL_VM_INITIAL_STACK_CAPACITY 256


typedef enum {
  TEL_VM_OK = 0,
  TEL_VM_ERROR = 1,
} Tel_VMResult;


typedef struct {
  Tel_Value *stack;
  size_t stack_capacity;
} Tel_VM;


void Tel_initVM(Tel_VM *vm);
void Tel_freeVM(Tel_VM *vm);

Tel_VMResult Tel_execute(Tel_VM *vm, Tel_Chunk const *chunk);
