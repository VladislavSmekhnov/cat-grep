#include <stdio.h>
#include <string.h>

typedef struct options {
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

int argv_parser(int argc, char** argv, opt* flags);

int main(int argc, char** argv) {
  int state = 0;
  if (argc > 1) {
    printf("argv[0] = %s\nargv[1] = %s\nargv[2] = %s\n", argv[0], argv[1], argv[2]);
    opt flags;
    zero_flags(&flags);
    if (argv_parser(argc, argv, &flags) == 0) {
      int count = 1;
      printf("%s\n", argv[count]);
      while (argv[count][1] != '\0') {
        FILE *fptr;
        if ((fptr = fopen(argv[count], "r")) != NULL) {
          char print_char = getc(fptr);
          char futur_char = '\0';
          int line_number = 1;
          // if we have n flag firstly run:
          if (print_char != EOF) {
            if (flags.n) {
              printf("%6d\t", line_number);
              line_number++;
            }
          }
          while ((futur_char = getc(fptr)) != EOF) {
            // if s:
            if (print_char == '\n' && futur_char == '\n') {
              while (futur_char == '\n') {
                futur_char = getc(fptr);
              }
            }
            if (print_char != EOF) {
              // print:
              if (flags.t) {
                if (flags.v) {
                  if (print_char >= 0 && print_char < 32)
                    printf("^%c", print_char + 64);
                  else if (print_char == '\t')
                    printf("^I");
                  else
                    printf("%c", print_char);
                } else if (print_char == '\t') {
                  printf("^I");
                } else {
                  printf("%c", print_char);
                }
              }
              if (flags.v) {
                if (print_char >= 0 && print_char < 32)
                  printf("^%c", print_char + 64);
                else
                  printf("%c", print_char);
              }
              // if n:
              if (flags.n) {
                printf("%6d\t", line_number);
                line_number++;
              }
              // if e:
              if (flags.e) {
                if (flags.v) {
                  if (print_char >= 0 && print_char < 32)
                    printf("^%c", print_char + 64);
                  else if (print_char == '\n')
                    printf("$");
                  else
                    printf("%c", print_char);
                } else if (print_char == '\n') {
                  printf("$");
                }
              }
            }
            print_char = futur_char;
          }
          if (print_char != EOF)
            printf("%c", print_char);

        } else {
          state = 1;
        }
      }
    }
  }
  return state;
}

int argv_parser(int argc, char** argv, opt* flags) {
  int count = 1;
  int state = 0;

  printf("Inside parser:\n");
  while (argv) {
    if (argv[count][0] == '-' && argv[count][1] != '-') {
      printf("strlen(argv[count]) = %ld\n", strlen(argv[count]));
      for (size_t i = 1; i < strlen(argv[count]); ++i) {
        printf("%c\n", argv[count][i]);
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
        } else if (i == 1) {
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
    count++;
  }
  for (int i = 1; i < argc; ++i) {
    memset(argv[i], '\0', strlen(argv[i]));
  }

  return state;
}
