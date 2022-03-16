#include "token.h"

token_t *init_token(char *value, token_type_t type)
{
  token_t *token = calloc(1, sizeof(token_t));
  token->token_value = value;
  token->token_type = type;
  return token;
}

void destroy_token(token_t *token)
{
  free(token);
}
