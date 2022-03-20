// cover as many lexemes as possible

typedef enum my_enum
{
  A, B, C, D, E
} my_enum;

typedef struct my_struct
{
  int val;
  struct my_struct *next;
} my_struct;

my_struct *who_is_the_next(my_struct *a_struct)
{
  if (a_struct)
    return a_struct->next;
  return ((void*)0);  // which is NULL
}

int main(int argc, char *argv[])
{
  /*
    this is a block comment
    I am comment :-)
  */

  // this is an inline comment

  float f1 = 3.14f;
  double f2 = 1e-5;

  int a = 2;
  int b = 1;
  int c, d;
  int _true = 1, _false = 0;

  if (_true && _false)   // and this is another inline comment
  {
    c = a + b;
    d = a * b;
  }
  else if (_true || _false)
  {
    c = a - b;
    d = a / b;
  }
  else
  {
    c = a & b;
    d = a | b;
  }

  // and this is an indentifier
  char i_am_id = 'c';
  char *yet_another_id = "well, hello lexer :^)";

  // sum of 0 to 10
  int i = 10;
  int sum = 0;
  while (i >= 0)
  {
    sum += i;
    i--;
  }

  // sum of 1 to 50
  sum = 0;
  for (int i = 1; i <= 50; i++)
  {
    sum += i;
  }

  my_struct a_struct;
  my_struct *the_next = who_is_the_next(&a_struct);
  int the_val = a_struct.val;

  return 0;
}
