#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>
#include <stdlib.h>

typedef enum token_type_t
{
  T_EOF,

  T_ID,

  T_LPAREN,
  T_RPAREN,
  T_LBRACKET,
  T_RBRACKET,
  T_LBRACE,
  T_RBRACE,
  T_COMMA,
  T_SEMICOLON,
  T_COLON,

  OP_ASSIGN,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  // OP_MOD,

  OP_EQUAL,
  OP_NEQ,
  OP_LEQ,
  OP_GEQ,
  OP_LT,
  OP_GT,

  K_IF,
  K_ELSE,
  // T_DO,
  // T_GOTO,
  K_WHILE,
  K_FOR,
  K_RETURN,

  K_INT,
  K_CHAR,
  // K_FLOAT,
  // K_DOUBLE,
  // K_BOOL,
  // K_TRUE,
  // K_FALSE,
  K_VOID,
} token_type_t;

typedef struct token_t
{
  char *token_value;
  token_type_t token_type;
} token_t;

token_t *init_token(char *value, token_type_t type);

void destroy_token(token_t *token);

#endif
