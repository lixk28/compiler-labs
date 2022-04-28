#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdio.h>

typedef enum token_type
{
  TK_PLUS,
  TK_MINUS,
  TK_MUL,
  TK_DIV,
  TK_LPAREN,
  TK_RPAREN,
  TK_NUM,
  TK_EOF
} token_type;

typedef struct token_t
{
  token_type type;
  char *loc;
  size_t len;
  long double val;  // used if token type is TK_NUM
  struct token_t *next;
} token_t;

token_t *lex(char *line);

void dump_token_list(token_t *head);

void destroy_token_list(token_t *head);

#endif