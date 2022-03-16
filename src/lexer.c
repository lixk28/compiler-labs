#include "lexer.h"
#include <stdio.h>

char *read_src(const char *file_path)
{
  FILE *src_file = fopen(file_path, "r");
  if (src_file == NULL)
  {
    printf("Cannot open file %s\n", file_path);
    exit(1);
  }

  fseek(src_file, 0L, SEEK_END);
  size_t file_size = ftell(src_file);
  fseek(src_file, 0L, SEEK_SET);
  #ifdef DEBUG
    printf("source file size is %d\n", file_size);
  #endif

  char *buf = calloc(file_size + 1, sizeof(char));
  fread(buf, sizeof(char), file_size, src_file);
  strcat(buf, "\0");
  #ifdef DEBUG
    printf("length of buffer: %d\n", strlen(buf));
    printf("source code:\n");
    printf("%s\n", buf);
  #endif

  fclose(src_file);

  return buf;
}

lexer_t *init_lexer(const char *file_path)
{
  lexer_t *lexer = (lexer_t *) calloc(1, sizeof(lexer_t));
  
  lexer->buf = read_src(file_path);
  lexer->src_size = strlen(lexer->buf);
  lexer->c = lexer->buf[0];
  lexer->i = 0;

  return lexer;
}

void destroy_lexer(lexer_t *lexer)
{
  free(lexer->buf);
  free(lexer);
}

char next_char(lexer_t *lexer)
{
  if (lexer->i < lexer->src_size && lexer->c != '\0')
  {
    lexer->i += 1;
    lexer->c = lexer->buf[lexer->i];
  }
  return lexer->c;
}

char peek_char(lexer_t *lexer, size_t offset)
{
  return lexer->buf[MIN(lexer->i + offset, lexer->src_size)];
}

void skip_char(lexer_t *lexer)
{
  // ignore these characters
  while (lexer->c == '\n' || lexer->c == '\r' || lexer->c == ' ' || lexer->c == '\t')
    next_char(lexer);
}

// FIXME: the logic isn't right, use lexer->c and next_char() carefully
void skip_comment(lexer_t *lexer)
{
  if (lexer->c == '/')
  {
    if (next_char(lexer) == '/') // skip inline comments
    {
      while (lexer->c != '\n')  // ignore all the comments until we meet a '\n'
        next_char(lexer);
    }

    if (next_char(lexer) == '*') // skip block comments
    {
      while (lexer->c != '*')
        next_char(lexer);
    }
  }
}

void scan(lexer_t *lexer)
{
  while (lexer->c != '\0')
  {
    skip_char(lexer);
    skip_comment(lexer);

    // TODO: parse id, numbers, char, string literal, keywords, operators
  }
}

void lexer()
{

}
