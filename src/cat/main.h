#ifndef SRC_CAT_MAIN_H_
#define SRC_CAT_MAIN_H_

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

int argv_parser(int argc, char *argv[], opt *flags, int *current_argv_number);
int execute_program(int argc, char *argv[], opt flags,
                    int *current_argv_number);
void output_file(FILE *file, opt flags);

#endif  // SRC_CAT_MAIN_H_