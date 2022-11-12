#include "main.h"

void zero_flags(cat_opt *flags) {
  flags->b = 0;
  flags->e = 0;
  flags->v = 0;
  flags->n = 0;
  flags->s = 0;
  flags->t = 0;
}

int main(int argc, char *argv[]) {
  int state = 0;

  if (argc > 1) {
    cat_opt flags;
    zero_flags(&flags);
    int count;
    if (argv_parser(argc, argv, &flags, &count) == 0) {
      state = execute_program(argc, argv, flags, &count);
    } else {
      state = 1;
      fprintf(stderr, "Incorrect arguments\n");
    }
  }

  return state;
}

int execute_program(int argc, char *argv[], cat_opt flags, int *count) {
  int state = 0;
  while (*count < argc && argv[*count][0] != '\0') {
    FILE *fptr = fopen(argv[*count], "r");
    if (fptr != NULL) {
      output_file(fptr, flags);
      fclose(fptr);
    } else {
      fprintf(stderr, "No shuch file or directory: %s\n", argv[*count]);
      state = 1;
    }
    *count++;
  }

  return state;
}

void output_file(FILE *file, cat_opt flags) {
  char futur_char, print_char;
  int count_s = 1;
  int count_b = 0;
  int count_n = 0;
  int i = 0;
  if (flags.b == 1) flags.n = 0;
  while ((futur_char = fgetc(file)) != EOF) {
    i++;
    if (flags.s == 1 && futur_char == '\n' && count_s < 3) count_s++;
    if (futur_char != '\n') count_s = 0;
    if (count_s == 3) continue;
    if ((flags.e == 1 && flags.v == 1) && (futur_char == '\n') &&
        (print_char != '\n') && (count_n != 0 || flags.n == 0) &&
        (count_b != 0 || flags.b == 0))
      printf("$");
    if (flags.n == 1 && (count_n == 0 || print_char == '\n'))
      printf("%6d\t", ++count_n);
    if ((flags.b == 1) && ((count_b == 0 && futur_char != '\n') ||
                           (print_char == '\n' && futur_char != '\n')))
      printf("%6d\t", ++count_b);
    if ((flags.e == 1 && flags.v == 1) && (futur_char == '\n') &&
        ((print_char == '\n') || (count_n == 1 && flags.n == 1 && i <= 1) ||
         (count_b == 0 && flags.b == 1 && i <= 1)))
      printf("$");
    print_char = futur_char;
    if (flags.t == 1 && flags.v == 1 && futur_char == '\t') {
      printf("^I");
      continue;
    }
    if (flags.v == 1 && futur_char >= 0 && futur_char < 32 &&
        futur_char != '\n' && futur_char != '\t') {
      printf("^%c", futur_char + 64);
      continue;
    }
    if (flags.v == 1 && futur_char == 127) {
      printf("^?");
      continue;
    }
    if (flags.e == 1 && flags.v == 1 && futur_char == 127) printf("^?");
    printf("%c", futur_char);
  }
}

int argv_parser(int argc, char *argv[], cat_opt *flags, int *count) {
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
