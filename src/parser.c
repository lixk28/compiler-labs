#include "parser.h"
#include "lexer.h"
#include "stack.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
         node->type == ND_LPAREN ||
         node->type == ND_RPAREN ||
         node->type == ND_NUM;
}

static bool match(node_t *node, token_t *token)
{
  return (node->type == ND_PLUS && token->type == TK_PLUS) ||
         (node->type == ND_MINUS && token->type == TK_MINUS) ||
         (node->type == ND_MUL && token->type == TK_MUL) ||
         (node->type == ND_DIV && token->type == TK_DIV) ||
         (node->type == ND_LPAREN && token->type == TK_LPAREN) ||
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

static void error(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

/*  expression grammar used in LL(1) parsing with left recursion removed
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
        error("error in LL(1) parsing, mismatched terminals");
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
              error("error in LL(1) parsing, unexpected token");
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
              error("error in LL(1) parsing, unexpected token");
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
              error("error in LL(1) parsing, unexpected token");
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
              error("error in LL(1) parsing, unexpected token");
          }
          break;
        case ND_FACTOR:
          switch (lookahead->type)
          {
            case TK_LPAREN:
              pop(stack, &curr);
              curr->children = calloc(3, sizeof(node_t *));
              curr->child_num = 3;
              curr->children[0] = new_op_node(ND_LPAREN);
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
              error("error in LL(1) parsing, unexpected token");
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

static void action_table(int *state, token_type tok, action_type *act, int *prod)
{
  switch (*state) {
    case 0:
    case 4:
    case 6:
    case 7:
    case 8:
    case 9:
      switch (tok) {
        case TK_LPAREN: *act = SHIFT; *state = 4; break;
        case TK_NUM: *act = SHIFT; *state = 5; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 1:
      switch (tok) {
        case TK_PLUS: *act = SHIFT; *state = 6; break;
        case TK_MINUS: *act = SHIFT; *state = 7; break;
        case TK_EOF: *act = ACCEPT; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 2:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 3; break;
        case TK_MUL: *act = SHIFT; *state = 8; break;
        case TK_DIV: *act = SHIFT; *state = 9; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 3:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_MUL:
        case TK_DIV:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 6; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 5:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_MUL:
        case TK_DIV:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 8; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 10:
      switch (tok) {
        case TK_PLUS: *act = SHIFT; *state = 6; break;
        case TK_MINUS: *act = SHIFT; *state = 7; break;
        case TK_RPAREN: *act = SHIFT; *state = 15; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 11:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 1; break;
        case TK_MUL: *act = SHIFT; *state = 8; break;
        case TK_DIV: *act = SHIFT; *state = 9; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 12:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 2; break;
        case TK_MUL: *act = SHIFT; *state = 8; break;
        case TK_DIV: *act = SHIFT; *state = 9; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 13:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_MUL:
        case TK_DIV:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 4; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 14:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_MUL:
        case TK_DIV:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 5; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    case 15:
      switch (tok) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_MUL:
        case TK_DIV:
        case TK_RPAREN:
        case TK_EOF: *act = REDUCE; *prod = 7; break;
        default: error("error in SLR(1) parsing, unexpected token"); break;
      } break;
    default:
      break;
  }
}

static void goto_table(int *state, node_type sym)
{
  switch (*state) {
    case 0:
      switch (sym) {
        case ND_EXPR: *state = 1; break;
        case ND_TERM: *state = 2; break;
        case ND_FACTOR: *state = 3; break;
        default: error("error in SLR(1) parsing, reduce error"); break;
      } break;
    case 4:
      switch (sym) {
        case ND_EXPR: *state = 10; break;
        case ND_TERM: *state = 2; break;
        case ND_FACTOR: *state = 3; break;
        default: error("error in SLR(1) parsing, reduce error"); break;
      } break;
    case 6:
      switch (sym) {
        case ND_TERM: *state = 11; break;
        case ND_FACTOR: *state = 3; break;
        default: error("error in SLR(1) parsing, reduce error"); break;
      } break;
    case 7:
      switch (sym) {
        case ND_TERM: *state = 12; break;
        case ND_FACTOR: *state = 3; break;
        default: error("error in SLR(1) parsing, reduce error"); break;
      } break;
    case 8:
      switch (sym) {
        case ND_FACTOR: *state = 13; break;
        default: error("error in SLR(1) parsing, reduce error"); break;
      } break;
    case 9:
      switch (sym) {
        case ND_FACTOR: *state = 13; break;
        default: break;
      } break;
    default:
      break;
  }
}

// this is so nasty...
static node_type toktype2nodetype(token_type tok)
{
  switch (tok)
  {
    case TK_PLUS: return ND_PLUS; break;
    case TK_MINUS: return ND_MINUS; break;
    case TK_MUL: return ND_MUL; break;
    case TK_DIV: return ND_DIV; break;
    case TK_LPAREN: return ND_LPAREN; break;
    case TK_RPAREN: return ND_RPAREN; break;
    case TK_NUM: return ND_NUM; break;
  }
}

static void reduce_and_transfer(stack_t *state_stack, stack_t *node_stack, int *curr_state, node_type reduce_to, size_t prod_len)
{
  node_t *node = new_node(reduce_to, NULL, 0);
  node->children = calloc(prod_len, sizeof(node_t *));
  node->child_num = prod_len;
  for (int i = node->child_num - 1; i >= 0; i--)
  {
    node_t *child;
    pop(node_stack, &child);
    node->children[i] = child;
  }
  push(node_stack, &node);

  for (int i = 0; i < node->child_num; i++)
    pop(state_stack, NULL);
  gettop(state_stack, curr_state);
  goto_table(curr_state, reduce_to);
  push(state_stack, curr_state);
}

/* expression grammar used in SLR(1)
 * (1) <expr>   ::= <expr> "+" <term>
 * (2)            | <expr> "-" <term>
 * (3)            | <term>
 * (4) <term>   ::= <term> "*" <factor>
 * (5)            | <term> "/" <factor>
 * (6)            | <factor>
 * (7) <factor> ::= "(" <expr> ")"
 * (8)            | "num"
 *
 */

node_t *slr1_parsing(token_t *token_list)
{
  stack_t *node_stack = new_stack(512, sizeof(node_t *));

  node_t *eof_node = new_node(ND_EOF, NULL, 0);

  push(node_stack, &eof_node);

  stack_t *state_stack = new_stack(512, sizeof(int));
  int curr_state = 0;
  push(state_stack, &curr_state);

  token_t *lookahead = token_list;

  int prod;

  action_type act;

  fprintf(stdout, "SLR(1) parsing procedure:\n");

  for (;;) {
    fprintf(stdout, "state stack: ");
    for (size_t i = 0; i < state_stack->size; i++)
      printf("%d%c", *(state_stack->bottom + state_stack->element_size * i), i == state_stack->size - 1 ? '\n' : ' ');

    fprintf(stdout, "node stack : ");
    for (size_t i = 0; i < node_stack->size; i++)
    {
      node_t *node;
      memcpy(&node, node_stack->bottom + node_stack->element_size * i, sizeof(node_t *));
      char tail = (i == node_stack->size - 1 ? '\n' : ' ');
      switch (node->type)
      {
        case ND_EXPR: printf("%s%c", "ND_EXPR", tail); break;
        case ND_TERM: printf("%s%c", "ND_TERM", tail); break;
        case ND_FACTOR: printf("%s%c", "ND_FACTOR", tail); break;
        case ND_PLUS: printf("%s%c", "ND_PLUS", tail); break;
        case ND_MINUS: printf("%s%c", "ND_MINUS", tail); break;
        case ND_MUL: printf("%s%c", "ND_MUL", tail); break;
        case ND_DIV: printf("%s%c", "ND_DIV", tail); break;
        case ND_LPAREN: printf("%s%c", "ND_LPAREN", tail); break;
        case ND_RPAREN: printf("%s%c", "ND_RPAREN", tail); break;
        case ND_NUM: printf("%s%c", "ND_NUM", tail); break;
        case ND_EOF: printf("%s%c", "ND_EOF", tail); break;
      }
    }

    printf("\n");

    gettop(state_stack, &curr_state);
    action_table(&curr_state, lookahead->type, &act, &prod);

    if (act == SHIFT)
    {
      push(state_stack, &curr_state);
      node_t *node = new_node(toktype2nodetype(lookahead->type), NULL, 0);
      if (node->type == ND_NUM)
        node->val = lookahead->val;
      push(node_stack, &node);
      lookahead = lookahead->next;
    }
    else if (act == REDUCE)
    {
      switch (prod)
      {
        case 1: // <expr> ::= <expr> "+" <term>
        case 2: // <expr> ::= <expr> "-" <term>
          reduce_and_transfer(state_stack, node_stack, &curr_state, ND_EXPR, 3);
          break;
        case 3: // <expr> ::= <term>
          reduce_and_transfer(state_stack, node_stack, &curr_state, ND_EXPR, 1);
          break;
        case 4: // <term> ::= <term> "*" <factor>
        case 5: // <term> ::= <term> "/" <factor>
          reduce_and_transfer(state_stack, node_stack, &curr_state, ND_TERM, 3);
          break;
        case 6: // <term> ::= <factor>
          reduce_and_transfer(state_stack, node_stack, &curr_state, ND_TERM, 1);
          break;
        case 7: // <factor> ::= "(" <expr> ")"
          reduce_and_transfer(state_stack, node_stack, &curr_state, ND_TERM, 3);
          break;
        case 8: // <factor> ::= "num"
          reduce_and_transfer(state_stack, node_stack, &curr_state, ND_FACTOR, 1);
          break;
      }
    }
    else if (act == ACCEPT)
    {
      fprintf(stdout, "ACCEPT!\n\n");
      break;
    }
    else
      error("error in SLR(1) parsing");
  }

  node_t *root;
  gettop(node_stack, &root);

  destroy_stack(node_stack);
  destroy_stack(state_stack);

  free(eof_node);

  return root;
}

static void dump_node(node_t *node, const char *prefix, const char *children_prefix)
{
  if (node)
  {
    fprintf(stdout, "%s", prefix);

    switch (node->type)
    {
      case ND_EXPR: fprintf(stdout, "%s\n", "expr"); break;
      case ND_EXPR_PRIME: fprintf(stdout, "%s\n", "expr'"); break;
      case ND_TERM: fprintf(stdout, "%s\n", "term"); break;
      case ND_TERM_PRIME: fprintf(stdout, "%s\n", "term'"); break;
      case ND_FACTOR: fprintf(stdout, "%s\n", "factor"); break;
      case ND_PLUS: fprintf(stdout, "%s: %c\n", "plus", '+'); break;
      case ND_MINUS: fprintf(stdout, "%s: %c\n", "minus", '-'); break;
      case ND_MUL: fprintf(stdout, "%s: %c\n", "mul", '*'); break;
      case ND_DIV: fprintf(stdout, "%s: %c\n", "div", '/'); break;
      case ND_LPAREN: fprintf(stdout, "%s: %c\n", "lparen", '('); break;
      case ND_RPAREN: fprintf(stdout, "%s: %c\n", "rparen", ')'); break;
      case ND_NUM: fprintf(stdout, "%s: %Lf\n", "num", node->val); break;
    }

    char *pointer, *segment;
    for (int i = 0; i < node->child_num; i++)
    {
      if (i == node->child_num - 1)
      {
        pointer = "└── ";
        segment = "    ";
      }
      else
      {
        pointer = "├── ";
        segment = "│   ";
      }

      char *my_prefix = malloc(strlen(children_prefix) + strlen(pointer) + 1);
      char *my_children_prefix = malloc(strlen(children_prefix) + strlen(segment) + 1);

      sprintf(my_prefix, "%s%s", children_prefix, pointer);
      sprintf(my_children_prefix, "%s%s", children_prefix, segment);

      dump_node(node->children[i], my_prefix, my_children_prefix);

      free(my_prefix);
      free(my_children_prefix);
    }
  }
}

void dump_parse_tree(node_t *root)
{
  fprintf(stdout, "parse tree dump:\n");
  if (root)
    dump_node(root, "", "");
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
