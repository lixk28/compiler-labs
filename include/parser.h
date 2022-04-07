#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include <stddef.h>

typedef enum ctype_t ctype_t;
enum ctype_t
{
  TY_VOID,
  TY_CHAR,
  TY_INT,
  TY_FLOAT,
  TY_DOUBLE,
  TY_ARRAY,
  TY_PTR,
  TY_STRUCT
};

typedef struct type_t type_t;
struct type_t
{
  ctype_t ctype;
  int size;     // sizeof

  type_t *ptr;  // pointer or array type
  int arr_len;  // array length

  // TODO: struct
};

typedef enum node_type_t node_type_t;
enum node_type_t
{
  ND_NUM,
  ND_ADD,     // +
  ND_SUB,     // -
  ND_NEG,     // unary -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_MOD,     // %
  ND_ASSIGN,  // =

  ND_EQ,      // ==
  ND_NEQ,     // !=
  ND_GT,      // >
  ND_LT,      // <
  ND_GEQ,     // >=
  ND_LEQ,     // <=
  ND_NOT,     // !
  ND_LOGAND,  // &&
  ND_LOGOR,   // ||

  ND_COMMA,   // ,
  ND_BLOCK,   // { ... }

  ND_VAR,     // variable

  ND_IF,      // if
  ND_WHILE,   // while
  ND_FOR,     // for

  ND_FUNC,
  ND_FUNCALL,
  ND_DECL,
};

typedef struct node_t node_t;
struct node_t
{
  token_t *token;
  node_type_t node_type;
  node_t *left;
  node_t *right;

};

node_t *new_node(node_type_t node_kind, node_t *left, node_t *right);

node_t *parse(token_t *tokens);

#endif
