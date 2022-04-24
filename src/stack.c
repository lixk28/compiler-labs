#include "stack.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

stack_t *new_stack(size_t capcity, size_t element_size)
{
  stack_t *stack = calloc(1, sizeof(stack_t));
  stack->bottom = malloc(element_size * capcity);
  stack->top = stack->bottom;
  stack->element_size = element_size;
  stack->capcity = capcity;
  stack->size = 0;
  return stack;
}

void push(stack_t *stack, void *element)
{
  if (stack)
  {
    if (stack->size == stack->capcity)
    {
      fprintf(stderr, "stack is full, element cannot be pushed\n");
      exit(1);
    }
    memcpy(stack->top, element, stack->element_size);
    stack->top += stack->element_size;
    stack->size += 1;
  }
}

void pop(stack_t *stack)
{
  if (stack)
  {
    if (stack->size == 0)
    {
      fprintf(stderr, "stack is empty, cannot pop from an empty stack\n");
      exit(1);
    }
    memset(stack->top - stack->element_size, 0, stack->element_size);
    stack->top -= stack->element_size;
    stack->size -= 1;
  }
}

void popn(stack_t *stack, size_t n)
{
  if (stack)
  {
    if (stack->size < n)
    {
      fprintf(stderr, "stack size %ld is smaller than %ld, cannot pop elements\n", stack->size, n);
      exit(1);
    }
    memset(stack->top - stack->element_size * n, 0, stack->element_size * n);
    stack->top -= stack->element_size * n;
    stack->size -= n;
  }
}

void top(stack_t *stack, void *top_element)
{
  if (stack)
  {
    if (stack->size == 0)
    {
      fprintf(stderr, "stack is empty, cannot get top of an empty stack\n");
      exit(1);
    }
    if (top_element)
      memcpy(top_element, stack->top - stack->element_size, stack->element_size);
  }
}

size_t size(stack_t *stack)
{
  return stack->size;
}

bool is_empty(stack_t *stack)
{
  return stack->size == 0 ? true : false;
}

void destroy_stack(stack_t * stack)
{
  free(stack->bottom);
  free(stack);
}
