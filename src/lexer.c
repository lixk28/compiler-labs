#include "lexer.h"
#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_t *lex(char *line)
{
  token_t head = {};
  token_t *curr = &head;

  char *p = line;
  for (;;)
  {
    // break when meeting line feed '\n'
    if (*p == '\n')
      break;

    // skip whitespaces
    if (*p == ' ' || *p == '\t' || *p == '\r')
    {
      p++;
      continue;
    }

    // operators
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')')
    {
      curr->next = calloc(1, sizeof(token_t));
      switch (*p)
      {
        case '+': curr->next->type = TK_PLUS; break;
        case '-': curr->next->type = TK_MINUS; break;
        case '*': curr->next->type = TK_MUL; break;
        case '/': curr->next->type = TK_DIV; break;
        case '(': curr->next->type = TK_LPAREN; break;
        case ')': curr->next->type = TK_RPAREN; break;
      }
      curr->next->loc = p;
      curr->next->len = 1;
      curr = curr->next;

      p++;
      continue;
    }

    // numbers
    if (isdigit(*p))
    {
      char *q = p++;
      while (isdigit(*p) || *p == '.')
        p++;

      char *end;
      long double val = strtold(q, &end);

      if (p != end)
      {
        fprintf(stdout, "invalid numeric value!\n");
        exit(1);
      }

      curr->next = calloc(1, sizeof(token_t));
      curr->next->type = TK_NUM;
      curr->next->loc = q;
      curr->next->len = p - q;
      curr->next->val = val;
      curr = curr->next;

      continue;
    }

    fprintf(stdout, "invalid expression!\n");
    break;
  }

  curr->next = calloc(1, sizeof(token_t));
  curr->next->type = TK_EOF;

  return head.next;
}

void dump_token_list(token_t *head)
{
  fprintf(stdout, "token list dump:\n");
  while (head)
  {
    switch (head->type)
    {
      case TK_EOF:
        fprintf(stdout, "<EOF>\n");
        break;
      default:
        fwrite(head->loc, sizeof(char), head->len, stdout);
        fputc('\n', stdout);
        break;
    }
    head = head->next;
  }
  fputc('\n', stdout);
}

void destroy_token_list(token_t *head)
{
  while (head != NULL)
  {
    token_t *temp = head;
    head = head->next;
    free(temp);
  }
}
