#include "lexer.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

static bool iskeyword(token_t *token)
{
  // ENHANCE:
  // * support more keywords :^)
  static char *keyword_list[] =
  {
    "if", "else", "do", "while", "goto", "continue", "break",
    "for", "return", "switch", "case", "default",
    "short", "int", "long", "char", "float", "double", "void"
  };

  for (size_t i = 0; i < sizeof(keyword_list) / sizeof(*keyword_list); i++)
  {
    if (!memcmp(keyword_list[i], token->loc, strlen(keyword_list[i])))
      return true;
  }
  return false;
}

token_t *lex(lexer_t *lexer)
{
  token_t head = {};
  token_t *curr = &head;

  while (*(lexer->p) != '\0')
  {
    skip_char(lexer);
    skip_comment(lexer);
    
    // numeric literals
    // ENHANCE:
    // read and convert binary, octal and hexadecimal number
    // currently we only read and convert decimal number
    if (isdigit(CURR_CHAR(lexer)) || (CURR_CHAR(lexer) == '.' && isdigit(PEEK_CHAR(lexer, 1))))
    {
      char *q = NEXT_CHAR(lexer);
      loop
      {
        if (isalnum(CURR_CHAR(lexer)) || CURR_CHAR(lexer) == '.')
          NEXT_CHAR(lexer);
        else if (CURR_CHAR(lexer) && PEEK_CHAR(lexer, 1) && \
                 strchr("eE", CURR_CHAR(lexer)) && strchr("+-", PEEK_CHAR(lexer, 1)))
          NEXT_NCHAR(lexer, 2);
        else
          break;
      }
      curr->next = make_token(q, lexer->p, T_NUM);
      curr = curr->next;
      continue;
    }

    // character literal
    if (CURR_CHAR(lexer) == '\'')
    {
      if (PEEK_CHAR(lexer, 1) == '\0') // unclosed char literal
      {
        // TODO: raise an error here
      }

      // ENHANCE:
      // escape character and utf-8 support
      char c = PEEK_CHAR(lexer, 1);

      char *q = strchr(lexer->p + 1, '\'');
      if (!q) // unclosed char literal
      {
        // TODO: raise an error here
      }

      curr->next = make_token(lexer->p, q, T_CHAR);
      curr->next->cval = c;
      curr = curr->next;
      NEXT_NCHAR(lexer, curr->len);
      continue;
    }

    // string literal
    if (CURR_CHAR(lexer) == '"')
    {
      if (PEEK_CHAR(lexer, 1) == '\0')  // unclosed string literal
      {
        // TODO: raise an error here
      }

      // ENHANCE:
      // * escape character and utf-8 support
      // * ignore single '\'

      char *q = strchr(lexer->p + 1, '"');
      if (!q) // unclosed string literal
      {
        // TODO: raise an error here
      }

      curr->next = make_token(lexer->p, q, T_STR);
      curr = curr->next;
      NEXT_NCHAR(lexer, curr->len);
      continue;
    }

    // identifiers or keywords
    // letter_ -> [A-Za-z_]
    // numbers -> [0-9]
    // id -> letter_ (letter_ | numbers)*
    // FIXME: specify keywords not implemented
    if (isalpha(CURR_CHAR(lexer)) || CURR_CHAR(lexer) == '_')
    {
      char *q = lexer->p;
      loop
      {
        NEXT_CHAR(lexer);
        if (isalnum(CURR_CHAR(lexer)) || CURR_CHAR(lexer) == '_')
          continue;
        else
          break;
      }
      curr->next = make_token(q, lexer->p - 1, T_ID);
      if (iskeyword(curr->next))
        curr->next->type = T_KEYWORD;
      curr = curr->next;
      NEXT_NCHAR(lexer, curr->len);
      continue;
    }
  }

  return head.next;
}

