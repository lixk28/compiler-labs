#include "kric.h"
#include "lexer.h"

// now we only have lexical analysis stage
// write the tokens to tokens.txt
void kric_compile(const char *file_path)
{
  lexer_t *lexer = init_lexer(file_path);

  scan(lexer);

  destroy_lexer(lexer);
}