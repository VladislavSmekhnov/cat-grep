#ifndef SRC_CAT_MAIN_H
#define SRC_CAT_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct options {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} opt;

int argv_parser(int argc, char *argv[], opt *flags, int *count);
int execute_program(int argc, char *argv[], opt flags, int *count);
void output_file(FILE *file, opt flags);

#endif  // SRC_CAT_MAIN_H