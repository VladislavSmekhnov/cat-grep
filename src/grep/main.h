#ifndef SRC_GREP_MAIN_H
#define SRC_GREP_MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct grep_options {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} grep_opt;

int argv_parser(int argc, char *argv[], grep_opt *flags, int *count);
void output_file(FILE *file, grep_opt flags);

#endif  // SRC_GREP_MAIN_H