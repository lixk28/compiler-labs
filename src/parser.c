#include "parser.h"
#include <stdlib.h>

node_t *new_node(node_kind_t node_kind)
{
  node_t *node = calloc(1, sizeof(node_t));
  node->node_kind = node_kind;
  return node;
}

static token_t *next_token(token_t *tokens)
{
  return tokens->next;
}

node_t *parse(token_t *tokens)
{
  node_t *root = new_node(ND_NUM);

  return root;
}
