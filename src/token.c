#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_t *make_token(char *start, char *end, token_type_t type)
{
  token_t *token = calloc(1, sizeof(token_t));
  token->loc = start;
  token->len = end - start + 1;
  token->type = type;
  return token;
}

static bool isequal(token_t *token, const char *tok_val)
{
  if (token->len == strlen(tok_val) && !memcmp(token->loc, tok_val, token->len))
    return true;
  else
    return false;;
}

bool eat(token_t **tokens_p, token_t *tokens, const char *tok_val)
{
  if (isequal(tokens, tok_val))
  {
    *tokens_p = tokens->next;
    return true;
  }
  return false;
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
      case T_NUM: fprintf(stdout, "Numeric: %ld or %Lf\n", head->ival, head->fval); break;
      case T_CHAR: fprintf(stdout, "Character: %c\n", head->cval); break;
      case T_STR: fprintf(stdout, "String: %s\n", head->sval); break;
      case T_EOF: fprintf(stdout, "End Of File\n"); break;
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
