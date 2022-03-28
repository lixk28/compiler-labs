#include "lexer.h"
#include "error.h"
#include "token.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static char *read_src(const char *file_path)
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

  dbgln("source file size is %d\n", file_size);

  char *buf = calloc(file_size + 1, sizeof(char));
  fread(buf, sizeof(char), file_size, src_file);
  strcat(buf, "\0");

  dbgln("length of buffer is %d\n", strlen(buf));

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
static bool start_with(char *p, const char *prefix)
{
  return strncmp(p, prefix, strlen(prefix)) == 0 ? true : false;
}

static bool is_keyword(token_t *token)
{
  // ENHANCE:
  // * support more keywords :^)
  static char *keyword_list[] =
  {
    "if", "else", "do", "while", "goto", "continue", "break", "for",
    "switch", "case", "default", "return", "void", "struct", "enum",
    "singed", "unsigned", "short", "int", "long", "char", "float", "double",
    "const", "typedef", "sizeof", "typeof"
  };

  for (size_t i = 0; i < sizeof(keyword_list) / sizeof(*keyword_list); i++)
  {
    if (strlen(keyword_list[i]) == token->len && \
        !memcmp(keyword_list[i], token->loc, strlen(keyword_list[i])))
      return true;
  }
  return false;
}

static int is_delimiter(char *p)
{
  // delimiters with one character
  static char delimiters_1[] =
  {
    '(', ')', '[', ']', '{', '}', ':', ';', ',', '.',
    '?', '!', '<', '>', '=',
    '+', '-', '*', '/', '%',
    '~', '&', '|'
  };

  // delimiters with multiple characters
  static char *delimiters_2[] =
  {
    "==", "!=", "<=", ">=", "->",
    "+=", "-=", "*=", "/=", "%=",
    "&&", "||",
    "++", "--",
    "&=", "|=", "^=", "<<", ">>", "<<=", ">>="
  };

  for (size_t i = 0; i < sizeof(delimiters_2) / sizeof(*delimiters_2); i++)
    if (start_with(p, delimiters_2[i]))
      return strlen(delimiters_2[i]);

  for (size_t i = 0; i < sizeof(delimiters_1) / sizeof(*delimiters_1); i++)
    if (*p == delimiters_1[i])
      return 1;
  return 0;
}

static char convert_char(token_t *token)
{
  char *p = token->loc;
  if (*p == '\\') // escape character
  {
    // reference https://en.wikipedia.org/wiki/Escape_sequences_in_C
    switch (*(p + 1))
    {
      case 'a': return '\a';
      case 'b': return '\b';
      case 'e': return '\e';
      case 'f': return '\f';
      case 'n': return '\n';
      case 'r': return '\r';
      case 't': return '\t';
      case 'v': return '\v';
      default: return *p;
    }
  }
  else  // normal character
    return *p;
}

static void convert_str(token_t *token)
{
  // TODO:
  // just implement it

  // ENHANCE:
  // * escape character support
  // * ignore single '\'
}

static bool convert_int(token_t *token)
{
  char *p = token->loc;

  // check if it's a binary, octal or hexadecimal number
  int base = 10;
  if (start_with(p, "0x") && isxdigit(*(p + 2)))  // hexadecimal
  {
    p += 2;
    base = 16;
  }
  else if (start_with(p, "0") && isdigit(*(p + 1))) // octal
  {
    p += 1;
    base = 8;
  }
  else if (start_with(p, "0b") && (*(p + 2) == '0' || *(p + 2) == '1')) // binary
  {
    p += 2;
    base = 2;
  }

  if (p != token->loc + token->len)
    return false;

  // TODO:
  // U, L and LL suffixes

  token->ival = strtoul(p, &p, base);
  return true;
}

static void convert_number(token_t *token)
{
  // try to convert it as an integer value
  if (convert_int(token))
    return;

  // if it's not an integer, it's a floating point number
  char *end;
  long double fval = strtold(token->loc, &end);

  // NOTE:
  // well, i think we can use *end to infer which type the floating number is
  // *end = f or F, type is float
  // *end = l or L, type is long double
  // else type is double

  if (token->loc + token->len != end)
    error_token(token, "invalid numeric value");

  token->fval = fval;
}

token_t *lex(lexer_t *lexer)
{
  token_t head = {};
  token_t *curr = &head;

  while (*(lexer->p) != '\0')
  {
    // skip whitespaces, line feeds, carraige returns, tabs
    if (*(lexer->p) == '\n' || *(lexer->p) == '\r' || *(lexer->p) == ' ' || *(lexer->p) == '\t')
    {
      NEXT_CHAR(lexer);
      continue;
    }

    if (start_with(lexer->p, "//")) // skip inline comment
    {
      NEXT_NCHAR(lexer, 2);
      while (*(lexer->p) != '\n')
        NEXT_CHAR(lexer);
      continue;
    }

    if (start_with(lexer->p, "/*")) // skip block comment
    {
      char *q = strstr(lexer->p + 2, "*/");
      if (!q) // unclosed block comment
      {
        error_at(lexer->buf, lexer->p, "unterminated block comment");
      }
      lexer->p = q + 2;
      continue;
    }

    // numeric literals
    if (isdigit(CURR_CHAR(lexer)) || (CURR_CHAR(lexer) == '.' && isdigit(PEEK_CHAR(lexer, 1))))
    {
      char *q = NEXT_CHAR(lexer);
      loop
      {
        if (CURR_CHAR(lexer) && PEEK_CHAR(lexer, 1) && \
            strchr("eE", CURR_CHAR(lexer)) && strchr("+-", PEEK_CHAR(lexer, 1)))
          NEXT_NCHAR(lexer, 2);
        else if (isalnum(CURR_CHAR(lexer)) || CURR_CHAR(lexer) == '.')
          NEXT_CHAR(lexer);
        else
          break;
      }
      curr->next = make_token(q, lexer->p - 1, T_NUM);
      convert_number(curr->next);
      curr = curr->next;
      continue;
    }

    // character literal
    if (CURR_CHAR(lexer) == '\'')
    {
      if (PEEK_CHAR(lexer, 1) == '\0') // unclosed char literal
      {
        error_at(lexer->buf, lexer->p, "unterminated character literal");
      }

      // ENHANCE:
      // escape character and utf-8 support
      char c = PEEK_CHAR(lexer, 1);

      char *q = strchr(lexer->p + 1, '\'');
      if (!q) // unclosed char literal
      {
        error_at(lexer->buf, lexer->p, "unterminated character literal");
      }

      curr->next = make_token(lexer->p, q, T_CHAR);
      curr->next->cval = convert_char(curr->next);
      curr = curr->next;
      NEXT_NCHAR(lexer, curr->len);
      continue;
    }

    // string literal
    if (CURR_CHAR(lexer) == '"')
    {
      if (PEEK_CHAR(lexer, 1) == '\0')  // unclosed string literal
      {
        error_at(lexer->buf, lexer->p, "unterminated string literal");
      }

      char *q = strchr(lexer->p + 1, '"');
      if (!q) // unclosed string literal
      {
        error_at(lexer->buf, lexer->p, "unterminated string literal");
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
      if (is_keyword(curr->next))
        curr->next->type = T_KEYWORD;
      curr = curr->next;
      continue;
    }

    // delimiters
    int delim_len = is_delimiter(lexer->p);
    if (delim_len > 0)
    {
      curr->next = make_token(lexer->p, lexer->p + delim_len - 1, T_DELIMITER);
      curr = curr->next;
      NEXT_NCHAR(lexer, curr->len);
      continue;
    }

    fprintf(stderr, "oh shoot, an invalid token!\n");
    exit(1);
  }

  return head.next;
}

