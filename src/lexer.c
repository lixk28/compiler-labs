#include "lexer.h"
#include <ctype.h>
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
  lexer->p = lexer->buf;
  lexer->src_size = strlen(lexer->buf);
  
  return lexer;
}

void destroy_lexer(lexer_t *lexer)
{
  free(lexer->buf);
  free(lexer);
}

// I take it from chibicc
static bool start_with(char *buf, const char *prefix)
{
  return strncmp(buf, prefix, strlen(prefix)) == 0 ? true : false;
}

void skip_char(lexer_t *lexer)
{
  // ignore these characters
  while (*(lexer->p) == '\n' || *(lexer->p) == '\r' || *(lexer->p) == ' ' || *(lexer->p) == '\t')
    NEXT_CHAR(lexer);
}

void skip_comment(lexer_t *lexer)
{
  if (start_with(lexer->buf, "//")) // skip inline comment
  {
    NEXT_NCHAR(lexer, 2);
    while (*(lexer->p) != '\n')
      NEXT_CHAR(lexer);
  }

  if (start_with(lexer->buf, "/*")) // skip block comment
  {
    NEXT_NCHAR(lexer, 2);
    char *q = strstr(lexer->p, "*/");
    if (!q) // unclosed block comment
    {
      // TODO: raise an error here
    }
    lexer->p = q;
    NEXT_NCHAR(lexer, 2);
  }
}

token_t *lex(lexer_t *lexer)
{
  token_t head = {};
  token_t *curr = &head;

  while (*(lexer->p) != '\0')
  {
    skip_char(lexer);
    skip_comment(lexer);

    // TODO: parse id, numbers, char, string literal, keywords, operators
    
    // numeric literals
    if (isdigit(CURR_CHAR(lexer)) || (CURR_CHAR(lexer) == '.' && isdigit(PEEK_CHAR(lexer, 1))))
    {
      
    }

    // character literal

    // string literal
  }

  return head.next;
}

