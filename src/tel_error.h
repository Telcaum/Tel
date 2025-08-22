#pragma once

#include "tel_lexer.h"


typedef struct {
  char const *source;
  bool logged_an_error;
} Tel_ErrorLogger;


void Tel_initErrorLogger(Tel_ErrorLogger *self, char const *source);

void Tel_logError(Tel_ErrorLogger *self, char const *msg, Tel_Token token);
