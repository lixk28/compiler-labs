int main()
{
  /*
    this is a block comment
    ...
  */
  int a = 2;
  int b = 1;
  int c, d;

  if (a == b)   // this is a inline comment
  {
    c = a + b;
  }
  else
  {
    d = a - b;
  }

  // and this is an indentifier
  char i_am_id = 'c';

  int i = 10;
  int sum = 0;
  while (i >= 0)
  {
    sum = sum + i;
    i = i - 1;
  }

  return 0;
}