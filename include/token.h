#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>
#include <stdint.h>

typedef enum token_type_t token_type_t;
enum token_type_t
{
  T_EOF,

  T_NUM,  // numeric literals
  T_CHAR, // character literals
  T_STR,  // string literals
  
  T_ID,         // identifiers
  T_KEYWORD,    // reserved words by C standard
  T_DELIMITER,  // everything including operators, parens, brackets, braces, commas, colons, semicolons
};

typedef struct token_t token_t;
struct token_t
{  
  char *loc;
  size_t len;
  token_type_t type;

  union // if type is T_NUM, it stores the value
  {
    int64_t ival;
    long double fval;
  };

  char cval;  // if type is T_CHAR_LITERAL, it stores the character
  char *sval; // if type is T_STR_LITERAL, it stores the string with null terminated

  token_t *next;  // tokens are stored as a single linked list
};

token_t *make_token(char *start, char *end, token_type_t type);

void print_token_list(token_t *head);

void destroy_token(token_t *head);

#endif
