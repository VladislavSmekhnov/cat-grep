#include "main.h"

/**
 * @brief Запускает программу в целом.
 *
 * @param argc количество аргументов командной строки
 * @param argv вектор (набор) аргументов командной строки
 * @return int код ошибки:
 * 0 - OK;
 * 1 - не удалось выделит память для корректной работы программы
 */
int main(int argc, char *argv[]) {
  int state = 0;
  opt flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL};
  if (get_memory(&flags) == 0) {
    sort_bash_first(argc, argv, &flags);
    sort_bash_second(argc, argv, &flags);
    make_pcre_patterns(&flags);
    free_memory(&flags);
  } else {
    state = 1;
  }
  return state;
}

void sort_bash_first(int argc, char *argv[], opt *flags) {
  int has_flags = 1;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0' && has_flags; j++) {
        has_flags = collect_flags(argv[i][j], flags);
      }
      has_flags = 1;
    }
  }
}

void sort_bash_second(int argc, char *argv[], opt *flags) {
  int out = 0;
  int letter_e = 0, letter_f = 0;
  int was_a_pattern = 0;

  int option = (flags->e || flags->f) ? 1 : 0;
  if (option) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        for (int j = 1; argv[i][j] != '\0' && !out; j++) {
          if (argv[i][j] == 'e' && argv[i][j + 1] != '\0') {
            add_pattern(&argv[i][j + 1], flags);
            out = 1;
          } else if (argv[i][j] == 'f' && argv[i][j + 1] != '\0') {
            add_pattern_from_file(&argv[i][j + 1], flags);
          } else if (argv[i][j] == 'e' && argv[i][j + 1] == '\0') {
            letter_e = 1;
          } else if (argv[i][j] == 'f' && argv[i][j + 1] == '\0') {
            letter_f = 1;
          }
        }
      } else {
        if (letter_f) {
          add_pattern_from_file(argv[i], flags);
          letter_f = 0;
        } else if (letter_e) {
          add_pattern(argv[i], flags);
          letter_e = 0;
        } else {
          add_file(argv[i], flags);
        }
      }
      out = 0;
    }
  } else {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-' && !was_a_pattern) {
        add_pattern(argv[i], flags);
        was_a_pattern = 1;
      } else if (argv[i][0] != '-') {
        add_file(argv[i], flags);
      }
    }
  }
}

/**
 * @brief Принимает флаги s21_grep (grep) из аргументов командной строки.
 *
 * @param ch символ
 * @param flags структура с флагами
 * @return int код ошибки:
 * 0 - OK;
 * 1 - некорректные аргументы командной строки;
 */
int collect_flags(char ch, opt *flags) {
  int state = 1;
  if (ch == 'e') {
    flags->e += 1;
    state = 0;
  } else if (ch == 'i') {
    flags->i += 1;
  } else if (ch == 'v') {
    flags->v += 1;
  } else if (ch == 'c') {
    flags->c += 1;
  } else if (ch == 'l') {
    flags->l += 1;
  } else if (ch == 'n') {
    flags->n += 1;
  } else if (ch == 'h') {
    flags->h += 1;
  } else if (ch == 's') {
    flags->s += 1;
  } else if (ch == 'f') {
    flags->f += 1;
    state = 0;
  } else if (ch == 'o') {
    flags->o += 1;
  }
  return state;
}

/**
 * @brief Выделяет необходимое количество памяти.
 *
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 * @return int код ошибки:
 * 0 - OK;
 * int > 0 - не удалось выделить память;
 */
int get_memory(opt *flags) {
  int state = 0;
  flags->files = (char **)calloc(2048, sizeof(char *));
  flags->patterns = (char *)calloc(2048, sizeof(char));
  if (flags->files == NULL) {
    state = 1;
    free(flags->files);
  }
  if (flags->patterns == NULL) {
    state = 2;
    free(flags->patterns);
  }
  if (flags->files != NULL && flags->patterns != NULL) {
    for (int i = 0; i < 2048 && state == 0; i++) {
      flags->files[i] = (char *)calloc(2048, sizeof(char));
      if (flags->files[i] == NULL) {
        for (int j = 0; j < i; j++) {
          free(flags->files[i]);
        }
        free(flags->files);
        free(flags->patterns);
        state = 2;
      }
    }
  } else {
    state = 1;
  }
  return state;
}

/**
 * @brief Очищает память.
 *
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 */
void free_memory(opt *flags) {
  for (size_t i = 0; i < 2048; ++i) {
    free(flags->files[i]);
  }
  free(flags->files);
  free(flags->patterns);
}

/**
 * @brief Добавляет набор символов для паттерна из файла.
 *
 * @param filename название файла
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 */
void add_pattern_from_file(char *filename, opt *flags) {
  FILE *stream = NULL;
  stream = fopen(filename, "r");
  char str[1024] = {0};
  if (stream) {
    while (!feof(stream)) {
      str[0] = '\0';
      fgets(str, sizeof(str), stream);
      for (int c = 0; str[c] != EOF && str[c] != '\0'; c++) {
        if (str[c] == '\n' && c > 0 && strlen(str) > 1) {
          str[c] = '\0';
        }
      }
      add_pattern(str, flags);
    }
    fclose(stream);
  } else {
    free_memory(flags);
    exit(1);
  }
}

/**
 * @brief Добавяет набор символов для паттерна s21_grep (grep).
 *
 * @param patternname набор символов для паттерна
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 */
void add_pattern(char *patternname, opt *flags) {
  if (flags->pattern_length != 0) {
    flags->patterns[flags->pattern_length++] = '|';
  }
  if (patternname[0] == ')') {
    flags->patterns[flags->pattern_length++] = '\'';
    flags->patterns[flags->pattern_length++] = '\\';
    flags->patterns[flags->pattern_length++] = ')';
    flags->patterns[flags->pattern_length++] = '\'';
  } else {
    strcpy(&flags->patterns[flags->pattern_length], patternname);
    flags->pattern_length += strlen(patternname);
  }
}

/**
 * @brief Увеличивает количество файлов в структуре флагов s21_grep (grep).
 *
 * @param filename название файла
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 */
void add_file(char *filename, opt *flags) {
  strcpy(flags->files[flags->files_number], filename);
  flags->files_number += 1;
}

/**
 * @brief Создаёт паттеры.
 *
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 */
void make_pcre_patterns(opt *flags) {
  char str[1024] = {0};
  str[0] = '\0';
  int buffer[1024] = {0};
  int count = 0, lines_amount = 0;
  int c_flag_amount = 0, out = 1;
  pcre *pattern = create_pattern(flags, flags->patterns);
  for (size_t i = 0; i < flags->files_number; i++) {
    FILE *fptr = fopen(flags->files[i], "r");
    if (fptr) {
      while (!(feof(fptr))) {
        lines_amount++;
        str[0] = '\0';
        fgets(str, sizeof(str), fptr);
        count = pcre_exec(pattern, NULL, str, strlen(str), 0, 0, buffer,
                          sizeof(buffer));
        if (count && strlen(str)) {
          if ((count > 0 && !(flags->v)) || (count < 0 && flags->v)) {
            if (flags->files_number > 1 && (!(flags->l) || flags->c) &&
                !(flags->h) && out) {
              printf("%s:", flags->files[i]);
            }
            if (flags->n && !(flags->l) && !(flags->c)) {
              printf("%d:", lines_amount);
            }
            if (flags->c) {
              out = 0;
              c_flag_amount++;
            }
            if (flags->l) {
              if (flags->c) {
                printf("%d\n", c_flag_amount);
              }
              printf("%s\n", flags->files[i]);
              break;
            }
            if (!(flags->c) && (!(flags->o) || (flags->v))) {
              printf("%s", str);
              if (str[strlen(str) - 1] != '\n') {
                printf("\n");
              }
            }
            if (flags->o && !(flags->c)) {
              run_flag_o(flags, str);
            }
          }
        }
      }
      if (count && flags->c && (!(flags->l) || !c_flag_amount)) {
        if (flags->files_number > 1 && !(flags->h) && c_flag_amount == 0) {
          printf("%s:", flags->files[i]);
        }
        printf("%d\n", c_flag_amount);
      }
      out = 1;
      c_flag_amount = 0;
      fclose(fptr);
    }
    lines_amount = 0;
  }
  if (pattern) {
    free(pattern);
  }
}

/**
 * @brief Создаёт объект паттерна для флага
 *
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 * @param pattername набор символов в паттерне
 * @return pcre* готовый паттерн
 */
pcre *create_pattern(opt *flags, char *pattername) {
  const char *error = NULL;
  int num_error = 0;
  pcre *pattern = NULL;
  if (flags->i) {
    pattern = pcre_compile(pattername, PCRE_CASELESS, &error, &num_error, NULL);
  } else {
    pattern = pcre_compile(pattername, 0, &error, &num_error, NULL);
  }
  if (!pattern) {
    free_memory(flags);
    free(pattern);
    exit(num_error);
  }
  return pattern;
}

/**
 * @brief Применяет флаг -o для s21_grep (grep): печатает только совпадающие
 * (непустые) части совпавшей строки.
 *
 * @param flags набор флагов s21_grep (grep) из аргументов командной строки
 * @param str строка, которую ищем
 */
void run_flag_o(opt *flags, char *str) {
  int count = 0;
  int buffer[1024] = {0};
  char *start_pos = NULL;
  int single_pattern_pos = 0;
  long unsigned int present_pos = 0;
  char single_pattern[1024] = {0};
  single_pattern[0] = '\0';
  start_pos = str;
  for (size_t c = 0; c <= strlen(flags->patterns); c++) {
    if (flags->patterns[c] == '|' || flags->patterns[c] == '\0') {
      single_pattern[single_pattern_pos] = '\0';
      single_pattern_pos = 0;
      pcre *pattern = create_pattern(flags, single_pattern);
      while ((strlen(start_pos) >= strlen(single_pattern)) &&
             (count = pcre_exec(pattern, NULL, start_pos, strlen(str), 0, 0,
                                buffer, sizeof(buffer))) > 0 &&
             (strlen(str) >= (present_pos + buffer[1] - 1))) {
        for (int i = buffer[0]; i < buffer[1]; i++) {
          putchar(start_pos[i]);
        }
        printf("\n");
        start_pos = &start_pos[buffer[1]];
        present_pos += buffer[1];
      }
      free(pattern);
    } else {
      single_pattern[single_pattern_pos++] = flags->patterns[c];
    }
  }
  start_pos = NULL;
}
