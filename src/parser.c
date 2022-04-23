#include "parser.h"
#include <stdlib.h>

// TODO: LL(1) parsing table and LR(1) parsing table

node_t *parse(token_t *token_list)
{
  node_t *root;

  // TODO: build the parse tree

  return root;
}

void destroy_parse_tree(node_t *root)
{
  if (root)
  {
    destroy_parse_tree(root->lhs);
    destroy_parse_tree(root->rhs);
    free(root);
  }
}
