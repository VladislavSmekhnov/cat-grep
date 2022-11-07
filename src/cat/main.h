#ifndef SRC_CAT_MAIN_H
#define SRC_CAT_MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cat_options {
  int b;
  int e;
  int v;
  int n;
  int s;
  int t;
} cat_opt;

int argv_parser(int argc, char *argv[], cat_opt *flags, int *count);
void output_file(FILE *file, cat_opt flags);

#endif  // SRC_CAT_MAIN_H
