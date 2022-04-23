#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum node_type
{
  ND_PLUS,
  ND_MINUS,
  ND_MUL,
  ND_DIV,
  ND_LPAREN,
  ND_RPAREN,
  ND_NUM
} node_type;

typedef struct node_t
{
  node_type type;
  token_t *token;
  struct node_t *lhs;
  struct node_t *rhs;
} node_t;

node_t *parse(token_t *token_list);

void dump_parse_tree(node_t *root);

void destroy_parse_tree(node_t *root);

#endif