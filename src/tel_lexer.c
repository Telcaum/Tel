#include <assert.h>
#include <string.h>

#include "tel_lexer.h"


void Tel_initLexer(Tel_Lexer *self, char const *source) {
  self->line = 1;
  self->indent = 0;
  self->is_counting_indent = true;
  self->source = source;
  self->pos = 0;
  self->start = 0;
}


static char cur(Tel_Lexer *self) {
  return self->source[self->pos];
}


static char peek(Tel_Lexer *self) {
  return self->source[self->pos + 1];
}


static void advance(Tel_Lexer *self) {
  self->pos++;
}


static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || c == '_';
}


static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}


static bool isAlNum(char c) {
  return isAlpha(c) || isDigit(c);
}


static void skipWhitespace(Tel_Lexer *self) {
  for (;;) {
    switch(cur(self)) {
      case ' ':
      case '\t':
      case '\r':
        if (self->is_counting_indent) {
          self->indent++;
        }
        advance(self);
        break;

      case '\n':
        self->is_counting_indent = true;
        self->indent = 0;
        advance(self);
        self->line++;
        break;

      case '*':
        if (peek(self) == '*') {
          while (cur(self) != '\n' && cur(self) != '\0') {
            advance(self);
          }
          break;
        }
        else {
          self->is_counting_indent = false;
          return;
        }

      default:
        self->is_counting_indent = false;
        return;
    }
  }
}


static Tel_Token newToken(Tel_Lexer *self, Tel_TokenType type) {
  Tel_Token token;
  token.type = type;
  token.line = self->line;
  token.indent = self->indent;
  token.start = self->start;
  token.length = self->pos - self->start;

  return token;
}


static Tel_Token scanNumber(Tel_Lexer *self) {
  while (isDigit(cur(self))) {
    advance(self);
  }
  if (cur(self) == '.') {
    do {
      advance(self);
    } while (isDigit(cur(self)));
  }

  return newToken(self, TEL_TT_NUMBER);
}


static Tel_Token scanName(Tel_Lexer *self) {
  while (isAlNum(cur(self))) {
    advance(self);
  }

  Tel_TokenType type = TEL_TT_NAME;

  int length = self->pos - self->start;
  char const *start = &self->source[self->start];
  switch (length) {
    case 2:
      if      (strncmp(start, "fn", 2) == 0) type = TEL_TT_FN;
      else if (strncmp(start, "if", 2) == 0) type = TEL_TT_IF;
      break;

    case 3:
      /**/ if (strncmp(start, "rec", 3) == 0) type = TEL_TT_REC;
      break;
  }

  return newToken(self, type);
}


static Tel_Token scanSymbol(Tel_Lexer *self) {
  char c = cur(self);
  advance(self);

  switch (c) {
    case '+': return newToken(self, TEL_TT_PLUS);
    case '-': return newToken(self, TEL_TT_MINUS);
    case '*': return newToken(self, TEL_TT_STAR);
    case '/': return newToken(self, TEL_TT_SLASH);
    case '(': return newToken(self, TEL_TT_LPAREN);
    case ')': return newToken(self, TEL_TT_RPAREN);
    case ':': return newToken(self, TEL_TT_COLON);
    case ',': return newToken(self, TEL_TT_COMMA);
    case '=': return newToken(self, TEL_TT_EQUAL);

    case '<':
      if (peek(self) == '>') {
        advance(self);
        return newToken(self, TEL_TT_DIAMOND);
      }
      return newToken(self, TEL_TT_LESS_THAN);
  }

  return newToken(self, TEL_TT_UNKNOWN);
}


Tel_Token Tel_scanToken(Tel_Lexer *self) {
  skipWhitespace(self);

  self->start = self->pos;

  char c = cur(self);

  if (c == '\0') {
    return newToken(self, TEL_TT_EOF);
  }

  if (isDigit(c) || (c == '.' && isDigit(peek(self)))) {
    return scanNumber(self);
  }

  if (isAlpha(c)) {
    return scanName(self);
  }

  return scanSymbol(self);
}


char const *Tel_getTokenTypeName(Tel_TokenType token_type) {
  switch (token_type) {
    #define _TEL_TT(name) case TEL_TT_ ## name: return #name;
    TEL_TOKEN_TYPE_ENUM
    #undef _TEL_TT
  }

  assert(0);
}
