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
  fprintf(stdout, "build/bin/kcalc <parsing> <verbose>\n");
  fprintf(stdout, "- <parsing>: ll or lr\n");
  fprintf(stdout, "- <verbose>: --verbose-on or --verbose-off\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  if (argc < 3)
    usage();

  const char *parsing = argv[1];
  const char *verbose = argv[2];

  bool verbose_cond;
  if (!strcmp(verbose, "--verbose-on"))
    verbose_cond = true;
  else if (!strcmp(verbose, "--verbose-off"))
    verbose_cond = false;
  else
  {
    fprintf(stdout, "unknown verbose option\n");
    exit(1);
  }

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

        node_t *parse_tree;
        if (!strcmp(parsing, "ll"))
          parse_tree = ll1_parsing(token_list, verbose_cond);
        else if (!strcmp(parsing, "lr"))
          parse_tree  = slr1_parsing(token_list, verbose_cond);
        else
        {
          fprintf(stderr, "unknown parsing option\n");
          exit(1);
        }

        dump_parse_tree(parse_tree);

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