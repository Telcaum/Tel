#pragma once


typedef enum {
  TEL_EXEC_OK = 0,
  TEL_EXEC_COMPILE_ERROR = 1,
  TEL_EXEC_RUNTIME_ERROR = 2,
} Tel_ExecutionResult;


Tel_ExecutionResult Tel_run(char const *source);
