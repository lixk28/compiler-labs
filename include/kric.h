#ifndef KRIC_H
#define KRIC_H

#ifdef DEBUG
  #include <stdio.h>
  #define dbgln(fmt, ...) \
          do { fprintf(stderr, "%s:%d:%s(): " fmt, \
                      __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while(0)
  #define dbgln_if(expr, fmt, ...) \
          do { if (expr) \
               fprintf(stderr, "%s:%d:%s(): " fmt, \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while(0)
#else
  #define dbgln(fmt, ...) ((void)0)
  #define dbgln_if(expr, fmt, ...) ((void)0)
#endif

// just keep it simple and stupid

void kric_compile(const char *file_path);

#endif