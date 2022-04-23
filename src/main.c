#define  _GNU_SOURCE  // for getline()
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main(int argc, char * argv[])
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
        printf("%s\n", line);
        // line is the input of lexer
        token_t *token_list = lex(line);
        dump_token_list(token_list);
        // lexer output to parser
        // node_t *parse_tree = parse(token_list);
        // parser generate tree, we then calculate the value

        // destroy token list
        destroy_token_list(token_list);
        // destroy parse tree
        // destroy_parse_tree(parse_tree);
        free(line);
        break;
      }
    }
  }

  return 0;
}