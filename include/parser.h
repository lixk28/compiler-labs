#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum node_type
{
  ND_EXPR, ND_EXPR_PRIME,
  ND_TERM, ND_TERM_PRIME,
  ND_FACTOR,

  ND_PLUS,
  ND_MINUS,
  ND_MUL,
  ND_DIV,
  ND_LPAREN,
  ND_RPAREN,
  ND_NUM,

  // syntax-directed translation for quadruple
  ND_QUAD_PLUS,
  ND_QUAD_MINUS,
  ND_QUAD_MUL,
  ND_QUAD_DIV,
  ND_PUSH_NUM,

  ND_EOF
} node_type;

typedef enum action_type
{
  SHIFT,
  REDUCE,
  ACCEPT,
  ERR
} action_type;

typedef struct node_t
{
  node_type type;
  struct node_t **children;
  int child_num;
  long double val;
} node_t;

typedef struct sem_t
{
  bool istemp;
  size_t t;
  long double val;
} sem_t;

node_t *ll1_parsing(token_t *token_list);

void dump_parse_tree(node_t *root);

void destroy_parse_tree(node_t *root);

#endif
