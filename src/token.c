#include "token.h"

token_t *make_token(char *start, char *end, token_type_t type)
{
  token_t *token = calloc(1, sizeof(token_t));
  token->loc = start;
  token->len = end - start;
  token->token_type = type;
  return token;
}

void destroy_token(token_t *token)
{
  free(token);
}
