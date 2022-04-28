#include "parser.h"
#include "lexer.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static node_t *new_node(node_type type, node_t **children, int child_num)
{
  node_t *node = calloc(1, sizeof(node_t));
  node->type = type;
  node->children = children;
  node->child_num = child_num;
  return node;
}

static node_t *new_op_node(node_type op_type)
{
  node_t *node = new_node(op_type, NULL, 0);
  return node;
}

static node_t *new_num_node(long double val)
{
  node_t *node = new_node(ND_NUM, NULL, 0);
  node->val = val;
  return node;
}

static bool isterminal(node_t *node)
{
  return node->type == ND_PLUS ||
         node->type == ND_MINUS ||
         node->type == ND_MUL ||
         node->type == ND_DIV ||
         node->type == ND_LAPREN ||
         node->type == ND_RPAREN ||
         node->type == ND_NUM;
}

static bool match(node_t *node, token_t *token)
{
  return (node->type == ND_PLUS && token->type == TK_PLUS) ||
         (node->type == ND_MINUS && token->type == TK_MINUS) ||
         (node->type == ND_MUL && token->type == TK_MUL) ||
         (node->type == ND_DIV && token->type == TK_DIV) ||
         (node->type == ND_LAPREN && token->type == TK_LPAREN) ||
         (node->type == ND_RPAREN && token->type == TK_RPAREN) ||
         (node->type == ND_NUM && token->type == TK_NUM);
  // return token1->len == token2->len &&
  //        !memcmp(token1->loc, token2->loc, token1->len);
}

static void expand(stack_t *stack, node_t *curr, int child_num, node_type children_type[])
{
  pop(stack, &curr);

  curr->child_num = child_num;
  curr->children = calloc(child_num, sizeof(node_t *));

  for (int i = 0; i < child_num; i++)
    curr->children[i] = new_node(children_type[i], NULL, 0);

  for (node_t **child = curr->children + curr->child_num - 1; child != curr->children; curr--)
    push(stack, child);
}

// LL(1) parsing table

// LL(1) parsing
node_t *ll1_parsing(token_t *token_list)
{
  stack_t *stack = new_stack(512, sizeof(node_t *));

  node_t *eof_node = new_node(ND_EOF, NULL, 0);
  node_t *root_node = new_node(ND_EXPR, NULL, 0);

  push(stack, &eof_node);
  push(stack, &root_node);

  token_t *lookahead = token_list;

  node_t *top;
  gettop(stack, &top);

  while (top->type != ND_EOF)
  {
    if (isterminal(top))  // top symbol is a terminal
    {
      if (match(top, lookahead)) // matches
      {
        pop(stack, NULL);
        lookahead = lookahead->next;
      }
      else  // doesn't match
      {
        fprintf(stderr, "error in LL(1) parsing, mismatched terminals\n");
        exit(1);
      }
    }
    else  // top symbol is a nonterminal
    {
      node_t *curr;
      switch (top->type)
      {
        case ND_EXPR:
          switch (lookahead->type)
          {
            case TK_LPAREN:
            case TK_NUM:
              pop(stack, &curr);
              curr->children = calloc(2, sizeof(node_t *));
              curr->child_num = 2;
              curr->children[0] = new_node(ND_TERM, NULL, 0);
              curr->children[1] = new_node(ND_EXPR_PRIME, NULL, 0);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            default:
              fprintf(stderr, "error in LL(1) parsing, unexpected token\n");
              exit(1);
          }
          break;
        case ND_EXPR_PRIME:
          switch (lookahead->type)
          {
            case TK_PLUS:
              pop(stack, &curr);
              curr->children = calloc(3, sizeof(node_t *));
              curr->child_num = 3;
              curr->children[0] = new_op_node(ND_PLUS);
              curr->children[1] = new_node(ND_TERM, NULL, 0);
              curr->children[2] = new_node(ND_EXPR_PRIME, NULL, 0);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            case TK_MINUS:
              pop(stack, &curr);
              curr->children = calloc(3, sizeof(node_t *));
              curr->child_num = 3;
              curr->children[0] = new_op_node(ND_MINUS);
              curr->children[1] = new_node(ND_TERM, NULL, 0);
              curr->children[2] = new_node(ND_EXPR_PRIME, NULL, 0);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            case TK_RPAREN:
            case TK_EOF:
              pop(stack, NULL);
              break;
            default:
              fprintf(stderr, "error in LL(1) parsing, unexpected token\n");
              exit(1);
          }
          break;
        case ND_TERM:
          switch (lookahead->type)
          {
            case TK_LPAREN:
            case TK_NUM:
              pop(stack, &curr);
              curr->children = calloc(2, sizeof(node_t *));
              curr->child_num = 2;
              curr->children[0] = new_node(ND_FACTOR, NULL, 0);
              curr->children[1] = new_node(ND_TERM_PRIME, NULL, 0);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            default:
              fprintf(stderr, "error in LL(1) parsing, unexpected token\n");
              exit(1);
          }
          break;
        case ND_TERM_PRIME:
          switch (lookahead->type)
          {
            case TK_PLUS:
            case TK_MINUS:
              pop(stack, NULL);
              break;
            case TK_MUL:
              pop(stack, &curr);
              curr->children = calloc(3, sizeof(node_t *));
              curr->child_num = 3;
              curr->children[0] = new_op_node(ND_MUL);
              curr->children[1] = new_node(ND_FACTOR, NULL, 0);
              curr->children[2] = new_node(ND_TERM_PRIME, NULL, 0);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            case TK_DIV:
              pop(stack, &curr);
              curr->children = calloc(3, sizeof(node_t *));
              curr->child_num = 3;
              curr->children[0] = new_op_node(ND_DIV);
              curr->children[1] = new_node(ND_FACTOR, NULL, 0);
              curr->children[2] = new_node(ND_TERM_PRIME, NULL, 0);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            case TK_RPAREN:
            case TK_EOF:
              pop(stack, NULL);
              break;
            default:
              fprintf(stderr, "error in LL(1) parsing, unexpected token\n");
              exit(1);
          }
          break;
        case ND_FACTOR:
          switch (lookahead->type)
          {
            case TK_LPAREN:
              pop(stack, &curr);
              curr->children = calloc(3, sizeof(node_t *));
              curr->child_num = 3;
              curr->children[0] = new_op_node(ND_LAPREN);
              curr->children[1] = new_node(ND_EXPR, NULL, 0);
              curr->children[2] = new_op_node(ND_RPAREN);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            case TK_NUM:
              pop(stack, &curr);
              curr->children = calloc(1, sizeof(node_t *));
              curr->child_num = 1;
              curr->children[0] = new_num_node(lookahead->val);
              for (node_t **child = curr->children + curr->child_num - 1; child >= curr->children; child--)
                push(stack, child);
              break;
            default:
              fprintf(stderr, "error in LL(1) parsing, unexpected token\n");
              exit(1);
          }
          break;
      }
    }

    gettop(stack, &top);
  }

  free(eof_node);
  destroy_stack(stack);
  return root_node;
}

static void dump_node(node_t *node, int indent)
{
  if (node)
  {
    for (int i = 0; i < indent; i++)
      fprintf(stdout, "  ");
    switch (node->type)
    {
      case ND_EXPR: fprintf(stdout, "`-%s\n", "expr"); break;
      case ND_EXPR_PRIME: fprintf(stdout, "`-%s\n", "expr'"); break;
      case ND_TERM: fprintf(stdout, "`-%s\n", "term"); break;
      case ND_TERM_PRIME: fprintf(stdout, "`-%s\n", "term'"); break;
      case ND_FACTOR: fprintf(stdout, "`-%s\n", "factor"); break;
      case ND_PLUS: fprintf(stdout, "`-%s\n", "plus"); break;
      case ND_MINUS: fprintf(stdout, "`-%s\n", "minus"); break;
      case ND_MUL: fprintf(stdout, "`-%s\n", "mul"); break;
      case ND_DIV: fprintf(stdout, "`-%s\n", "div"); break;
      case ND_LAPREN: fprintf(stdout, "`-%s\n", "lparen"); break;
      case ND_RPAREN: fprintf(stdout, "`-%s\n", "rparen"); break;
      case ND_NUM: fprintf(stdout, "`-%s=%Lf\n", "num", node->val); break;
    }
    for (int i = 0; i < node->child_num; i++)
      dump_node(node->children[i], indent + 1);
  }
}

void dump_parse_tree(node_t *root)
{
  fprintf(stdout, "parse tree dump:\n");
  if (root)
    dump_node(root, 0);
}

void destroy_parse_tree(node_t *root)
{
  if (root)
  {
    for (int i = 0; i < root->child_num; i++)
      destroy_parse_tree(root->children[i]);
    free(root->children);
    free(root);
  }
}
