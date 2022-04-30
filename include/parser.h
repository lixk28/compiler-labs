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

node_t *ll1_parsing(token_t *token_list, bool verbose_cond);

node_t *slr1_parsing(token_t *token_list, bool verbose_cond);

void dump_parse_tree(node_t *root);

void destroy_parse_tree(node_t *root);

#endif