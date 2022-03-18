#include "token.h"
#include <stdio.h>
#include <stdlib.h>

token_t *make_token(char *start, char *end, token_type_t type)
{
  token_t *token = calloc(1, sizeof(token_t));
  token->loc = start;
  token->len = end - start + 1;
  token->type = type;
  return token;
}

void print_token_list(token_t *head)
{
  while (head)
  {
    fwrite(head->loc, sizeof(char), head->len, stdout);
    fputc('\t', stdout);

    switch (head->type)
    {
      case T_ID: fprintf(stdout, "Identifier\n"); break;
      case T_KEYWORD: fprintf(stdout, "Keyword\n"); break;
      case T_DELIMITER: fprintf(stdout, "Delimiter\n"); break;
      case T_NUM: fprintf(stdout, "Numeric\n"); break;
      case T_CHAR: fprintf(stdout, "Character\n"); break;
      case T_STR: fprintf(stdout, "String\n"); break;
      default: fprintf(stdout, "invalid token\n"); break;
    }

    head = head->next;
  }
}

void destroy_token(token_t *head)
{
  while (head)
  {
    token_t *temp = head;
    head = head->next;
    free(temp);
  }
}
