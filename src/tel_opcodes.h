#ifndef _TEL_OP
  // make the LS shut up
  #define _TEL_OP(name)
#endif // it doesn't work lol
// edit it does work with clang i love you

// R(X): the Xth register
// C(X): the Xth constant
// X   : a 1-byte operand
// X.X : a 2-byte operand

// - - - -> does nothing lol
_TEL_OP(RETURN)

// A B.B -> R(A) := C(B)
_TEL_OP(LOAD_CONST)

// A B - -> R(A) := -R(B)
_TEL_OP(NEGATE)

// A B C -> R(A) := R(B) + R(C)
_TEL_OP(ADD)

// A B C -> R(A) := R(B) - R(C)
_TEL_OP(SUB)

// A B C -> R(A) := R(B) * R(C)
_TEL_OP(MUL)

// A B C -> R(A) := R(B) / R(C)
_TEL_OP(DIV)

// A B - -> R(A) := R(B)
_TEL_OP(MOVE)
