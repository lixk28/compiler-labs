#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef enum node_kind_t node_kind_t;
enum node_kind_t
{
  ND_NUM,
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
};

typedef struct node_t node_t;
struct node_t
{
  token_t *token;
  node_kind_t node_kind;
  node_t *left;
  node_t *right;

};

node_t *new_node(node_kind_t node_kind);

node_t *parse(token_t *tokens);

#endif
