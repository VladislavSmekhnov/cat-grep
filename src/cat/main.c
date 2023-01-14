#include "main.h"

/**
 * @brief Запускает программу в целом.
 *
 * @param argc количество аргументов командной строки
 * @param argv вектор (набор) аргументов командной строки
 * @return int код ошибки:
 * 0 - OK;
 * 1 - файл не найден / некорректные аргументы командной строки при запуске
 * программы;
 */
int main(int argc, char *argv[]) {
  int state = 0;

  if (argc > 1) {
    opt flags = {0};
    int current_argv_number;
    if (argv_parser(argc, argv, &flags, &current_argv_number) == 0) {
      state = execute_program(argc, argv, flags, &current_argv_number);
    } else {
      state = 1;
      fprintf(stderr, "Incorrect arguments\n");
    }
  }

  return state;
}

/**
 * @brief Запускает программу считывания аргументов командной строки и вывода
 * файла.
 *
 * @param argc количество аргументов командной строки
 * @param argv вектор (набор) аргументов командной строки
 * @param flags флаги для s21_cat (cat)
 * @param current_argv_number текущий номер аргумента командной строки
 * @return int код ошибки:
 * 0 - OK;
 * 1 - файл для считывания/вывода не найден;
 */
int execute_program(int argc, char *argv[], opt flags,
                    int *current_argv_number) {
  int state = 0;
  while (*current_argv_number < argc && argv[*current_argv_number][0] != '\0') {
    FILE *fptr = fopen(argv[*current_argv_number], "r");
    if (fptr != NULL) {
      output_file(fptr, flags);
      fclose(fptr);
    } else {
      fprintf(stderr, "No shuch file or directory: %s\n",
              argv[*current_argv_number]);
      state = 1;
    }
    *current_argv_number += 1;
  }

  return state;
}

/**
 * @brief Выводит файл на консоль (в поток out).
 *
 * @param file файл, который выводим
 * @param flags аргументы cat для вывода файла
 */
void output_file(FILE *file, opt flags) {
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

/**
 * @brief Парсер аргументов командной строки.
 *
 * @param argc количество аргументов командной строки
 * @param argv вектор аргументов командной строки
 * @param flags флаги s21_cat (cat)
 * @param current_argv_number номер текущего аргумента
 * @return int код ошибки:
 * 0 - OK;
 * int > 0 - Ошибка в написании аргумента командной строки;
 */
int argv_parser(int argc, char *argv[], opt *flags, int *current_argv_number) {
  int tmp_argv_number = 1;
  int state = 0;

  while (tmp_argv_number < argc) {
    if (argv[tmp_argv_number][0] == '-' && argv[tmp_argv_number][1] != '-') {
      for (size_t i = 1; i < strlen(argv[tmp_argv_number]); ++i) {
        if (argv[tmp_argv_number][i] == 'b') {
          flags->b = 1;
        } else if (argv[tmp_argv_number][i] == 'e') {
          flags->e = 1;
          flags->v = 1;
        } else if (argv[tmp_argv_number][i] == 'E') {
          flags->e = 1;
        } else if (argv[tmp_argv_number][i] == 'n') {
          flags->n = 1;
        } else if (argv[tmp_argv_number][i] == 's') {
          flags->s = 1;
        } else if (argv[tmp_argv_number][i] == 't') {
          flags->t = 1;
          flags->v = 1;
        } else if (argv[tmp_argv_number][i] == 'T') {
          flags->t = 1;
        } else if (i == 1) {
          state = 1;
          break;
        }
      }
      memset(argv[tmp_argv_number], '\0', strlen(argv[tmp_argv_number]));
    } else if (argv[tmp_argv_number][0] == '-' &&
               argv[tmp_argv_number][1] == '-') {
      if (strcmp(argv[tmp_argv_number], "--number-nonblank") == 0) {
        flags->b = 1;
      } else if (strcmp(argv[tmp_argv_number], "--number") == 0) {
        flags->n = 1;
      } else if (strcmp(argv[tmp_argv_number], "--squeeze-blank") == 0) {
        flags->s = 1;
      } else {
        state = 2;
      }
      memset(argv[tmp_argv_number], '\0',
             strlen(argv[tmp_argv_number]));  // Erase arguments
    } else {
      // No arguments
      break;
    }
    tmp_argv_number++;
  }
  *current_argv_number = tmp_argv_number;

  return state;
}
