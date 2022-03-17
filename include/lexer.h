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

char *read_src(const char *file_path);

lexer_t *init_lexer(const char *file_path);

void destroy_lexer(lexer_t *lexer);

// skip whitespace, newline, return and tab
void skip_char(lexer_t *lexer);

// skip comments, including inline comments and block comments
void skip_comment(lexer_t *lexer);

void scan(lexer_t *lexer);

#endif
