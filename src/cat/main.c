#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int argv_parser(int argc, char* argv[], opt* flags, int *count);

int main(int argc, char* argv[]) {
  int state = 0;
  if (argc > 1) {
    opt flags;
    zero_flags(&flags);
    int count;
    if (argv_parser(argc, argv, &flags, &count) == 0) {
      int line_number = 1;
      while (count < argc && argv[count][0] != '\0') {
        char *filename = argv[count];
        int opened_file = 0;
        FILE *fptr = fopen(filename, "r");
        if (fptr != NULL) {
          opened_file++;
          char print_char = getc(fptr);
          char futur_char = '\0';
          // if we have n-flag firstly run:
          if (print_char != EOF) {
            if ((flags.n == 1 || flags.b == 1) && opened_file == 1) {
              if (flags.b == 1 && futur_char != '\n') {
                printf("%6d\t", line_number);
                line_number++;
              } else if (flags.b == 0) {
                printf("%6d\t", line_number);
                line_number++;
              }
            }
            while ((futur_char = getc(fptr)) != EOF) {
              // if s:
              if (flags.s == 1) {
                if (print_char == '\n' && futur_char == '\n') {
                  while (futur_char == '\n') {
                    futur_char = getc(fptr);
                  }
                }
                // if sn:
                if (print_char == '\n')
                  printf("%c", print_char);
                if (flags.n == 1 && print_char == '\n') {
                  printf("%6d\t", line_number);
                  line_number++;
                }
                // if se:
                if (flags.e == 1) {
                  if (flags.v == 1) {
                    if (print_char >= 0 && print_char < 32 && print_char != '\n' && print_char != '\t')
                      printf("^%c", print_char + 64);
                    else if (print_char == '\n')
                      printf("$");
                  } else if (print_char == '\n') {
                    printf("$");
                  }
                }
              }
              // print:
              if (flags.t == 1) {
                if (flags.v == 1) {
                  if (print_char >= 0 && print_char < 32 && print_char != '\n' && print_char != '\t')
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
              } else {
                printf("%c", print_char);
              }
              // if n:
              if ((flags.n == 1 || flags.b == 1) && print_char == '\n' && futur_char != EOF) {
                if (flags.b == 1 && futur_char != '\n') {
                  printf("%6d\t", line_number);
                  line_number++;
                } else if (flags.b == 0) {
                  printf("%6d\t", line_number);
                  line_number++;
                }
              }
              // if e:
              if (flags.e == 1) {
                if (flags.v == 1) {
                  if (print_char >= 0 && print_char < 32 && print_char != '\n' && print_char != '\t')
                    printf("^%c", print_char + 64);
                  else if (futur_char == '\n')
                    printf("$");
                } else if (futur_char == '\n') {
                  printf("$");
                }
              }

              print_char = futur_char;
            }
          }
          if (print_char != EOF)
            printf("%c", print_char);
          fclose(fptr);
          // if (argv[count + 1][0] != '\0')
          count++;
        } else {
          state = 1;
          printf("No such file or directory\n");
        }
      }
    }
  }
  return state;
}

int argv_parser(int argc, char* argv[], opt* flags, int *count) {
  int tmp_count = 1;
  int state = 0;

  while (tmp_count < argc) {
    if (argv[tmp_count][0] == '-' && argv[tmp_count][1] != '-') {
      for (size_t i = 1; i < strlen(argv[tmp_count]); ++i) {
        // printf("%c\n", argv[tmp_count][i]);
        if (argv[tmp_count][i] == 'b') {
          flags->b = 1;
        } else if (argv[tmp_count][i] == 'e') {
          flags->e = 1;
          flags->v = 1;
        } else if (argv[tmp_count][i] == 'E') {
          flags->e = 1;
        } else if (argv[tmp_count][i] == 'n') {
          flags->n = 1;
        } else if (argv[tmp_count][i] == 's') {
          flags->s = 1;
        } else if (argv[tmp_count][i] == 't') {
          flags->t = 1;
          flags->v = 1;
        } else if (argv[tmp_count][i] == 'T') {
          flags->t = 1;
        } else if (i == 1) {
          state = 1;
          break;
        }
      }
      // printf("argv[%d] = %s\n", tmp_count, argv[tmp_count]);
      memset(argv[tmp_count], '\0', strlen(argv[tmp_count]));
    } else if (argv[tmp_count][0] == '-' && argv[tmp_count][1] == '-') {
      if (strcmp(argv[tmp_count], "--number-nonblank") == 0) {
        flags->b = 1;
      } else if (strcmp(argv[tmp_count], "--number") == 0) {
        flags->n = 1;
      } else if (strcmp(argv[tmp_count], "--squeeze-blank") == 0) {
        flags->s = 1;
      } else {
        state = 2;
      }
      // printf("argv[%d] = %s\n", tmp_count, argv[tmp_count]);
      memset(argv[tmp_count], '\0', strlen(argv[tmp_count]));
    } else {
      break;
    }
    // printf("argv[%d] = %s\n", tmp_count, argv[tmp_count]);
    tmp_count++;
  }
  *count = tmp_count;
  return state;
}
