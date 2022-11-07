#include "main.h"

void zero_flags(opt *flags) {
  flags->b = 0;
  flags->e = 0;
  flags->v = 0;
  flags->n = 0;
  flags->s = 0;
  flags->t = 0;
}

int argv_parser(int argc, char *argv[], opt *flags, int *count);
void output_file(FILE *file, opt flags);

int main(int argc, char *argv[]) {
  int state = 0;

  if (argc > 1) {
    opt flags;
    zero_flags(&flags);
    int count;
    if (argv_parser(argc, argv, &flags, &count) == 0) {
      while (count < argc && argv[count][0] != '\0') {
        FILE *fptr = fopen(argv[count], "r");
        if (fptr != NULL) {
          output_file(fptr, flags);
          fclose(fptr);
        } else {
          fprintf(stderr, "No shuch file or directory: %s\n", argv[count]);
          state = 1;
        }
        count++;
      }
    } else {
      state = 1;
      fprintf(stderr, "Incorrect arguments\n");
    }
  }

  return state;
}

void output_file(FILE *file, opt flags) {
  char ch, ch_pred;
  int count_s = 1;
  int count_b = 0;
  int count_n = 0;
  int i = 0;
  if (flags.b == 1) flags.n = 0;
  while ((ch = fgetc(file)) != EOF) {
    i++;
    if (flags.s == 1 && ch == '\n' && count_s < 3) count_s++;
    if (ch != '\n') count_s = 0;
    if (count_s == 3) continue;
    if ((flags.e == 1 && flags.v == 1) && (ch == '\n') && (ch_pred != '\n') &&
        (count_n != 0 || flags.n == 0) && (count_b != 0 || flags.b == 0))
      printf("$");
    if (flags.n == 1 && (count_n == 0 || ch_pred == '\n'))
      printf("%6d\t", ++count_n);
    if ((flags.b == 1) &&
        ((count_b == 0 && ch != '\n') || (ch_pred == '\n' && ch != '\n')))
      printf("%6d\t", ++count_b);
    if ((flags.e == 1 && flags.v == 1) && (ch == '\n') &&
        ((ch_pred == '\n') || (count_n == 1 && flags.n == 1 && i <= 1) ||
         (count_b == 0 && flags.b == 1 && i <= 1)))
      printf("$");
    ch_pred = ch;
    if (flags.t == 1 && flags.v == 1 && ch == '\t') {
      printf("^I");
      continue;
    }
    if (flags.v == 1 && ch >= 0 && ch < 32 && ch != '\n' && ch != '\t') {
      printf("^%c", ch + 64);
      continue;
    }
    if (flags.v == 1 && ch == 127) {
      printf("^?");
      continue;
    }
    if (flags.e == 1 && flags.v == 1 && ch == 127) printf("^?");
    printf("%c", ch);
  }
}

int argv_parser(int argc, char *argv[], opt *flags, int *count) {
  int tmp_count = 1;
  int state = 0;

  while (tmp_count < argc) {
    if (argv[tmp_count][0] == '-' && argv[tmp_count][1] != '-') {
      for (size_t i = 1; i < strlen(argv[tmp_count]); ++i) {
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
      memset(argv[tmp_count], '\0',
             strlen(argv[tmp_count]));  // Erase arguments
    } else {
      // No arguments
      break;
    }
    tmp_count++;
  }
  *count = tmp_count;

  return state;
}
