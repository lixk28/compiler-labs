#include "error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void error_at(char *buf, char *loc, const char *fmt, ...)
{
  // count line number
  size_t line_num = 1;
  for (char *p = buf; p != loc; p++)
    if (*p == '\n')
      line_num++;

  // find the start of line
  char *line_start = loc;
  while (buf < line_start && *(line_start - 1) != '\n')
    line_start--;

  // find the end of line
  char *line_end = loc;
  while (*line_end && *line_end != '\n')
    line_end++;

  // count column number
  size_t col_num = 1;
  for (char *p = line_start; p != loc; p++)
    col_num++;

  va_list ap;
  va_start(ap, fmt);

  fprintf(stderr, WHITE "%ld:%ld: " RESET, line_num, col_num);  // print line number and column number
  fprintf(stderr, RED "error: " RESET);
  fprintf(stderr, fmt, ap); // error msg here
  fputc('\n', stderr);

  int indent = fprintf(stderr, "%*s%ld | ", 4, "", line_num);
  fwrite(line_start, sizeof(char), (size_t)(line_end - line_start), stderr);  // print line
  fputc('\n', stderr);

  fprintf(stderr, "%*s| ", indent - 2, "");
  fprintf(stderr, "%*s"RED"^"RESET, (int)(loc - line_start), ""); // print prompt char
  fputc('\n', stderr);

  va_end(ap);
  exit(1);
}

// void error_token(token_t *token, const char *fmt, ...)
// {

// }
