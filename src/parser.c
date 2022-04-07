#include "parser.h"
#include <stdlib.h>

node_t *new_node(node_type_t node_type, node_t *left, node_t *right)
{
  node_t *node = calloc(1, sizeof(node_t));
  node->node_type = node_type;
  node->left = left;
  node->right = right;
  return node;
}

static token_t *next_token(token_t *tokens)
{
  return tokens->next;
}

node_t *parse(token_t *tokens)
{

}
