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
      curr->next->type = TK_OP;
      curr->next->op = *p;
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
      curr->next->val = val;
      curr = curr->next;

      continue;
    }

    fprintf(stdout, "invalid expression!\n");
    break;
  }

  return head.next;
}

void dump_token_list(token_t *head)
{
  fprintf(stdout, "token list dump:\n");
  while (head)
  {
    switch (head->type)
    {
      case TK_OP: fprintf(stdout, "%c\n", head->op); break;
      case TK_NUM: fprintf(stdout, "%Lf\n", head->val); break;
    }
    head = head->next;
  }
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
