#include "kric.h"
#include "lexer.h"
#include "token.h"

// now we only have lexical analysis stage
// write the tokens to tokens.txt
void kric_compile(const char *file_path)
{
  lexer_t *lexer = init_lexer(file_path);

  token_t *tokens = lex(lexer);

  print_token_list(tokens);

  destroy_lexer(lexer);
}