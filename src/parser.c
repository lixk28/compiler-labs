#include "parser.h"
#include <stdlib.h>

/* expression grammar represented in BNF
 * <expr>   ::= <expr> "+" <term>
 *            | <expr> "-" <term>
 *            | <term>
 * <term>   ::= <term> "*" <factor>
 *            | <term> "/" <factor>
 *            | <factor>
 * <factor> ::= "(" <expr> ")"
 *            | "num"
 */

/*  grammar used in LL(1) parsing with left recursion removed
 *
 *  (1)  <expr>   ::= <term> <expr'>
 *  (2)  <expr'>  ::= "+" <term> <expr'>
 *  (3)             | "-" <term> <expr'>
 *  (4)             | ""
 *  (5)  <term>   ::= <factor> <term'>
 *  (6)  <term'>  ::= "*" <factor> <term'>
 *  (7)             | "/" <factor> <term'>
 *  (8)             | ""
 *  (9)  <factor> ::= "(" <expr> ")"
 *  (10)            | "num"
 *
 */

// LL(1) parsing table

// LR(1) parsing table


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
