#include "tel_vm.h"
#include "tel_debug.h"


void Tel_initVM(Tel_VM *vm) {
  vm->stack = TEL_ALLOCN(Tel_Value, TEL_VM_INITIAL_STACK_CAPACITY);
  TEL_CHECK_OOM(vm->stack);
  vm->stack_capacity = TEL_VM_INITIAL_STACK_CAPACITY;
}


void Tel_freeVM(Tel_VM *vm) {
  TEL_FREE(vm->stack);
  vm->stack = NULL;
  vm->stack_capacity = 0;
}


static void growStack(Tel_VM *vm) {
  vm->stack = TEL_REALLOCN(vm->stack, Tel_Value, (vm->stack_capacity *= 2));
  TEL_CHECK_OOM(vm->stack);
}


Tel_VMResult Tel_execute(Tel_VM *vm, Tel_Chunk const *chunk) {
  #ifdef __cplusplus
    // register keyword is deprecated in C++
    #define register
  #endif

  register Tel_Inst const *ip = chunk->code.data;
  register Tel_Inst current_inst;

  #undef register


  #ifdef TEL_DEBUG_VM_BOUNDS_CHECK
    #define READ_INST() (assert(ip - chunk->code.data < chunk->code.length), *ip++)
    #define READ_STACK(at) (assert((at) < vm->stack_capacity), vm->stack[at])
  #else
    #define READ_INST() (*ip++)
    #define READ_STACK(at) (vm->stack[at])
  #endif

  #define WRITE_STACK(at, v) do { \
    size_t index = (at); \
    while (index >= vm->stack_capacity) { \
      growStack(vm); \
    } \
    vm->stack[index] = (v); \
  } while (0)


  #ifdef TEL_DEBUG_TRACE_EXECUTION
    #define DEBUG_PRINT_INST(index) printInstruction(chunk, (index))
    #define DEBUG_PRINT_STACK(until) do { \
      printf("        : "); \
      for (int i = 0; i <= (int)(until); i++) { \
        printf("[%d: %g]", i, vm->stack[i].number); \
      } \
      puts(""); \
    } while (0)

  #else
    #define DEBUG_PRINT_INST(index)
    #define DEBUG_PRINT_STACK(until)
  #endif


  #if TEL_COMPUTED_GOTOS

    static void *dispatch_table[] = {
      #define _TEL_OP(name) &&label_ ## name,
      #include "tel_opcodes.h"
      #undef _TEL_OP
    };

    #define LOOP DISPATCH();
    #define CASE(name) label_ ## name

    #define DISPATCH() do { \
      DEBUG_PRINT_INST(ip - chunk->code.data); \
      goto *dispatch_table[TEL_GET_INST_OP_CODE(current_inst = READ_INST())]; \
    } while (0)

  #else
    #define LOOP loop: DEBUG_PRINT_INST(ip - chunk->code.data); switch (TEL_GET_INST_OP_CODE(current_inst = READ_INST()))
    #define CASE(name) case TEL_OP_ ## name
    #define DISPATCH() goto loop

  #endif


  LOOP {
    // - - - -> does nothing lol
    CASE(RETURN):
      return TEL_VM_OK;

    // A B.B -> R(A) := C(B)
    CASE(LOAD_CONST): {
      Tel_Inst to = TEL_GET_A__(current_inst);
      Tel_Inst constant_index = TEL_GET__BB(current_inst);
      WRITE_STACK(to, chunk->constants.data[constant_index]);
      DEBUG_PRINT_STACK(to);
      DISPATCH();
    }

    // A B - -> R(A) := -R(B)
    CASE(NEGATE): {
      Tel_Inst to = TEL_GET_A__(current_inst);
      Tel_Inst from = TEL_GET__B_(current_inst);
      WRITE_STACK(to, TEL_NUMBER(-READ_STACK(from).number));
      DEBUG_PRINT_STACK(to > from ? to : from);
      DISPATCH();
    }

    #define BIN_OP(op) do { \
      Tel_Inst a = TEL_GET_A__(current_inst); \
      Tel_Inst b = TEL_GET__B_(current_inst); \
      Tel_Inst c = TEL_GET___C(current_inst); \
      WRITE_STACK(a, TEL_NUMBER(READ_STACK(b).number op READ_STACK(c).number)); \
      DEBUG_PRINT_STACK((b > c ? b : c) > a ? (b > c ? b : c) : a); \
    } while (0)

    // A B C -> R(A) := R(B) + R(C)
    CASE(ADD):
      BIN_OP(+);
      DISPATCH();

    // A B C -> R(A) := R(B) - R(C)
    CASE(SUB):
      BIN_OP(-);
      DISPATCH();

    // A B C -> R(A) := R(B) * R(C)
    CASE(MUL):
      BIN_OP(*);
      DISPATCH();

    // A B C -> R(A) := R(B) / R(C)
    CASE(DIV):
      BIN_OP(/);
      DISPATCH();

    #undef BIN_OP

    // A B - -> R(A) := R(B)
    CASE(MOVE): {
      Tel_Inst to = TEL_GET_A__(current_inst);
      Tel_Inst from = TEL_GET__B_(current_inst);
      WRITE_STACK(to, TEL_NUMBER(READ_STACK(from).number));
      DEBUG_PRINT_STACK(to > from ? to : from);
      DISPATCH();
    }
  }

  fprintf(stderr, "Unknown instruction %d.\n", TEL_GET_INST_OP_CODE(current_inst));
  return TEL_VM_ERROR;
}
