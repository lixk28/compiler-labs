#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>
#include <stdlib.h>

typedef enum token_type_t token_type_t;
enum token_type_t
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
  
  OP_NOT,
  OP_LOGAND,
  OP_LOGOR,
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
  K_FLOAT,
  K_DOUBLE,
  // K_BOOL,
  // K_TRUE,
  // K_FALSE,
  K_VOID,
};

typedef struct token_t token_t;
struct token_t
{  
  char *loc;
  size_t len;
  token_type_t type;

  token_t *next;  // tokens are stored as a single linked list
};

token_t *make_token(char *start, char *end, token_type_t type);

void destroy_token(token_t *token);

#endif
