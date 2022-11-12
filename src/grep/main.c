#include "main.h"

void zero_flags(grep_opt *flags) {
  flags->e = 0;
  flags->i = 0;
  flags->v = 0;
  flags->c = 0;
  flags->l = 0;
  flags->n = 0;
  flags->h = 0;
  flags->s = 0;
  flags->f = 0;
  flags->o = 0;
}

int main(int argc, char *argv[]) {
  int state = 0;

  if (argc > 1) {
    grep_opt flags;
    zero_flags(&flags);
    int count;
    if (argv_parser(argc, argv, &flags, &count) == 0) {
      //
    } else {
      state = 2;
      fprintf(stderr, "Incorrect arguments\n");
    }
  } else {
    state = 1;
    fprintf(stderr, "There is no file or arguments\n");
  }

  return state;
}