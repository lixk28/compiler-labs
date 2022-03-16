#include <stdio.h>
#include "kric.h"
#include "lexer.h"

void usage()
{
  printf("Usage:\n");
  printf("./kric <file>\n");
}

int main(int argc, char *argv[])
{
  if (argc < 2)
    usage();

  kric_compile(argv[1]);

  return 0;
}