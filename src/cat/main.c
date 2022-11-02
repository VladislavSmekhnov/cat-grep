#include <stdio.h>
#include <string.h>

typedef struct opt{
  int b;
  int e;
  int v;
  int n;
  int s;
  int t;
} opt;

void zero_flags(opt *flags) {
  flags->b = 0;
  flags->e = 0;
  flags->v = 0;
  flags->n = 0;
  flags->s = 0;
  flags->t = 0;
}

int argv_parser(char** argv, opt* flags);

int main(int argc, char** argv) {
  if (argc > 1) {
    opt flags;
    zero_flags(&flags);
    argv_parser(argv, &flags);
  }
  return 0;
}

int argv_parser(char** argv, opt* flags) {
  int count = 1;
  int state = 0;

  while (argv) {
    if (argv[count][0] == '-' && argv[count][1] != '-') {
      for (size_t i = 0; i < strlen(argv[count]); ++i) {
        if (argv[count][i] == 'b') {
          flags->b = 1;
        } else if (argv[count][i] == 'e') {
          flags->e = 1;
          flags->v = 1;
        } else if (argv[count][i] == 'E') {
          flags->e = 1;
        } else if (argv[count][i] == 'n') {
          flags->n = 1;
        } else if (argv[count][i] == 's') {
          flags->s = 1;
        } else if (argv[count][i] == 't') {
          flags->t = 1;
          flags->v = 1;
        } else if (argv[count][i] == 'T') {
          flags->t = 1;
        } else {
          state = 1;
        }
      }
    } else if (argv[count][0] == '-' && argv[count][1] == '-') {
      if (strcmp(argv[count], "--number-nonblank") == 0) {
        flags->b = 1;
      } else if (strcmp(argv[count], "--number") == 0) {
        flags->n = 1;
      } else if (strcmp(argv[count], "--squeeze-blank") == 0) {
        flags->s = 1;
      } else {
        state = 2;
      }
    }
    memset(argv[count], '\0', strlen(argv[count]));
    count++;
  }

  return state;
}
