#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "tel_parser.h"
#include "tel_error.h"
#include "tel_lexer.h"
#include "tel_node.h"


static void advance(Tel_Parser *self) {
  self->previous_token = self->current_token;
  self->current_token = Tel_scanToken(self->lexer);
}


void Tel_initParser(Tel_Parser *self, Tel_Lexer *lexer, Tel_ErrorLogger *error_logger) {
  self->error_logger = error_logger;
  self->lexer = lexer;
  self->is_panicking = false;

  advance(self); // fill current token
}


static void logError(Tel_Parser *self, char const *message) {
  if (self->is_panicking) return;
  self->is_panicking = true;
  Tel_logError(self->error_logger, message, self->current_token);
}


static char const *getSource(Tel_Parser *self) {
  return self->lexer->source;
}


static Tel_Node *buildExpression(Tel_Parser *self);


// grouping ::= '(' expression ')'
static Tel_Node *buildGrouping(Tel_Parser *self) {
  assert(self->current_token.type == TEL_TT_LPAREN);
  advance(self);

  Tel_Node *node = buildExpression(self);

  if (self->current_token.type != TEL_TT_RPAREN) {
    logError(self, "Expected a closing ')'.");
  }
  else {
    advance(self);
  }

  return node;
}


// number ::= NUMBER
static Tel_Node *buildNumber(Tel_Parser *self) {
  assert(self->current_token.type == TEL_TT_NUMBER);

  Tel_Node *node = Tel_newNode(TEL_NT_NUMBER, self->current_token, (Tel_NodeAs){
    .number = { strtod(&getSource(self)[self->current_token.start], NULL) }
  });
  advance(self);
  return node;
}


// atom ::= number | grouping
static Tel_Node *buildAtom(Tel_Parser *self) {
  switch (self->current_token.type) {
    case TEL_TT_NUMBER:
      return buildNumber(self);

    case TEL_TT_LPAREN:
      return buildGrouping(self);

    default:
      logError(self, "Expected an expression.");
      return NULL;
  }
}


static bool isSuffix(Tel_TokenType token_type) {
  switch (token_type) {
    default:
      return false;
  }
}


// suffixExpr ::= atom (@NO_NEWLINE suffix)*
static Tel_Node *buildSuffixExpression(Tel_Parser *self) {
  Tel_Node *node = buildAtom(self);

  while (self->current_token.line == self->previous_token.line && isSuffix(self->current_token.type)) {
    assert(0 && "Not implemented");
  }

  return node;
}


static bool isPrefix(Tel_TokenType token_type) {
  switch (token_type) {
    case TEL_TT_MINUS:
      return true;

    default:
      return false;
  }
}


// prefixExpr ::= prefix prefixExpr | suffixExpr
static Tel_Node *buildPrefixExpression(Tel_Parser *self) {
  if (isPrefix(self->current_token.type)) {
    Tel_Token prefix = self->current_token;
    advance(self);
    Tel_Node *arg = buildPrefixExpression(self);
    return Tel_newNode(TEL_NT_PREFIX, prefix, (Tel_NodeAs){
      .prefix = { arg }
    });
  }

  return buildSuffixExpression(self);
}


// primaryExpr ::= prefixExpr
static Tel_Node *buildPrimaryExpression(Tel_Parser *self) {
  return buildPrefixExpression(self);
}


typedef enum {
  PREC_NONE,
  PREC_EQUALITY,
  PREC_SUM,
  PREC_FACTOR,
} Precedence;


static Precedence getPrecedence(Tel_TokenType token_type) {
  switch (token_type) {
    case TEL_TT_PLUS:
    case TEL_TT_MINUS:
      return PREC_SUM;

    case TEL_TT_STAR:
    case TEL_TT_SLASH:
      return PREC_FACTOR;

    case TEL_TT_EQUAL:
    case TEL_TT_DIAMOND:
      return PREC_EQUALITY;

    default:
      return PREC_NONE;
  }
}


// infixExpr ::= * magic *
static Tel_Node *buildInfixExpressionRHS(Tel_Parser *self, Tel_Node *lhs, Precedence below_min_precedence) {
  for (;;) {
    Tel_Token op = self->current_token;

    Precedence current_precedence = getPrecedence(op.type);
    if (current_precedence <= below_min_precedence) {
      return lhs;
    }

    advance(self);

    Tel_Node *rhs = buildPrimaryExpression(self);

    Precedence next_precedence = getPrecedence(self->current_token.type);
    if (next_precedence > current_precedence) {
      rhs = buildInfixExpressionRHS(self, rhs, (Precedence)(next_precedence - 1));
    }

    lhs = Tel_newNode(TEL_NT_INFIX, op, (Tel_NodeAs){
      .infix = { lhs, rhs }
    });
  }
}


// expression ::= infixExpr
static Tel_Node *buildExpression(Tel_Parser *self) {
  Tel_Node *lhs = buildPrimaryExpression(self);
  return buildInfixExpressionRHS(self, lhs, PREC_NONE);
}


static void recover(Tel_Parser *self) {
  self->is_panicking = false;
  for (;;) {
    switch (self->current_token.type) {
      case TEL_TT_NUMBER:
      case TEL_TT_LPAREN:
      case TEL_TT_MINUS:
      case TEL_TT_EOF:
        return;

      default:
        advance(self);
        break;
    }
  }
}


Tel_Node *Tel_buildTreeBranch(Tel_Parser *self) {
  for (;;) {
    if (self->is_panicking) {
      recover(self);
    }

    if (self->current_token.type == TEL_TT_EOF) return NULL;

    Tel_Node *node = buildExpression(self);
    if (node != NULL) return node;
  }
}
