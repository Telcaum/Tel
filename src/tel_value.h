#pragma once

#include "tel_memory.h"


#define TEL_NUMBER(n) ((Tel_Value){ .number = (n) })


typedef double Tel_Number;

typedef union {
  double number;
} Tel_Value;


#define TEL_VALUES_EQUAL(v1, v2) ((v1).number == (v2).number)


typedef TEL_LIST(Tel_Value) Tel_ValueList;


#define TEL_INIT_VALUE_LIST(l) TEL_LIST_INIT((l), Tel_Value)
#define TEL_FREE_VALUE_LIST(l) TEL_LIST_FREE(l)
#define TEL_WRITE_VALUE(value_list, value) TEL_LIST_PUSH((value_list), (value), Tel_Value)
