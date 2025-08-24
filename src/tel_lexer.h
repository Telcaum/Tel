#pragma once

#include <stdbool.h>

#include "tel_memory.h"


#define TEL_TOKEN_TYPE_ENUM \
  _TEL_TT(NAME) \
  _TEL_TT(NUMBER) \
  \
  _TEL_TT(FN) \
  _TEL_TT(IF) \
  _TEL_TT(REC) \
  \
  _TEL_TT(PLUS) \
  _TEL_TT(MINUS) \
  _TEL_TT(STAR) \
  _TEL_TT(SLASH) \
  _TEL_TT(LPAREN) \
  _TEL_TT(RPAREN) \
  _TEL_TT(COLON) \
  _TEL_TT(COMMA) \
  _TEL_TT(EQUAL) \
  _TEL_TT(DIAMOND) \
  _TEL_TT(LESS_THAN) \
  \
  _TEL_TT(UNKNOWN) \
  _TEL_TT(EOF) \


typedef enum {
  #define _TEL_TT(name) TEL_TT_ ## name,
  TEL_TOKEN_TYPE_ENUM
  #undef _TEL_TT
} Tel_TokenType;


typedef struct {
  Tel_TokenType type;
  int line;
  int indent;
  int start;
  int length;
} Tel_Token;


typedef TEL_LIST(Tel_Token) Tel_TokenList;


typedef struct {
  bool is_counting_indent;
  int line;
  int indent;
  int pos;
  int start;
  char const *source;
} Tel_Lexer;


void Tel_initLexer(Tel_Lexer *self, char const *source);

Tel_Token Tel_scanToken(Tel_Lexer *self);

char const *Tel_getTokenTypeName(Tel_TokenType token_type);
