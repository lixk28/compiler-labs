#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "kric.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define loop for(;;)

#define CURR_CHAR(LEXER) *((LEXER)->p)
#define PEEK_CHAR(LEXER, N) *((LEXER)->p + (N))
#define NEXT_CHAR(LEXER) (LEXER)->p++
#define NEXT_NCHAR(LEXER, N) (LEXER)->p += (N)

// FIXME:
// currently we have two problems with lexer:
// * we use a single buffer to store the entire source code, a better way is to 
//   set fixed size buffer like 4096, which is the block size
// * we can only read and compile one single file, no multiple files support
typedef struct lexer_t
{
  char *buf;
  char *p;
  size_t src_size;
} lexer_t;

lexer_t *init_lexer(const char *file_path);

void destroy_lexer(lexer_t *lexer);

token_t *lex(lexer_t *lexer);

#endif
