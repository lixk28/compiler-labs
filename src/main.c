#define  _GNU_SOURCE  // for getline()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"

void usage()
{
  fprintf(stdout, "usage:\n");
  fprintf(stdout, "build/bin/kcalc\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  printf("hello, kcalc\n");

  for (;;)
  {
    fprintf(stdout, "> "); // prompt
    for (;;)
    {
      char *line = NULL;
      size_t len;
      if (getline(&line, &len, stdin) == -1)
        printf("no line\n");
      else
      {
        // printf("%s\n", line);
        // line is the input of lexer
        token_t *token_list = lex(line);
        // dump_token_list(token_list);

        node_t *parse_tree = ll1_parsing(token_list);

        // destroy token list
        destroy_token_list(token_list);
        // destroy parse tree
        destroy_parse_tree(parse_tree);
        free(line);
        break;
      }
    }
  }

  return 0;
}
