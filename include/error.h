#ifndef ERROR_H
#define ERROR_H

#include "token.h"
#include <stdarg.h>

// taken from https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
// reference https://en.wikipedia.org/wiki/ANSI_escape_code
// boldface with coloring text
#define WHITE   "\x1b[1m\x1b[97m"
#define RED     "\x1b[1m\x1b[31m"
#define GREEN   "\x1b[1m\x1b[32m"
#define YELLOW  "\x1b[1m\x1b[33m"
#define BLUE    "\x1b[1m\x1b[34m"
#define MAGENTA "\x1b[1m\x1b[35m"
#define CYAN    "\x1b[1m\x1b[36m"
#define RESET   "\x1b[0m"

// raise an error on tokenization stage
void error_at(char *buf, char *loc, const char *fmt, ...) 
     __attribute__((__noreturn__, format(printf, 3, 4)));

// raise an error when parsing a token
void error_token(token_t *token, const char *fmt, ...)
     __attribute__((__noreturn__, format(printf, 2, 3)));

#endif
