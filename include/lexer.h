#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "kric.h"

#include <ctype.h>    // for isalpha() and isdigit()
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // for strcmp() and strncmp()
#include <stdbool.h>  // for bool

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

typedef struct lexer_t
{
  char *buf;
  size_t src_size;
  char c;
  size_t i;
} lexer_t;

// read the source code and put it in a buffer
char *read_src(const char *file_path);

lexer_t *init_lexer(const char *file_path);

void destroy_lexer(lexer_t *lexer);

// move to the next character and return it
char next_char(lexer_t *lexer);

// peek a char
char peek_char(lexer_t *lexer, size_t offset);

// char roll_back(lexer_t *lexer);

// skip whitespace, newline, return and tab
void skip_char(lexer_t *lexer);

// skip comments, including inline comments and block comments
void skip_comment(lexer_t *lexer);

#endif
