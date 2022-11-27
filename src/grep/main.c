#include "main.h"

int main(int argc, char *argv[]) {
  int error = 0;
  opt flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL};
  error = get_memory(&flags);
  if (!error) {
    sort_bash_first(argc, argv, &flags);
    sort_bash_second(argc, argv, &flags);
    make_pcre_patterns(&flags);
    free_memory(&flags);
  }
  return 0;
}

void sort_bash_first(int argc, char *argv[], opt *flags) {
  int result = 1;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0' && result; j++) {
        result = find_flags(argv[i][j], flags);
      }
      result = 1;
    }
  }
}

void sort_bash_second(int argc, char *argv[], opt *flags) {
  int i = 0, j = 0;
  int option = 0, out = 0;
  int point_e = 0, point_f = 0;
  int was_there_a_pattern = 0;

  option = (flags->e || flags->f) ? 1 : 0;
  if (option == 1) {
    for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        for (j = 1; argv[i][j] != '\0' && !out; j++) {
          if (argv[i][j] == 'e' && argv[i][j + 1] != '\0') {
            add_pattern(&argv[i][j + 1], flags);
            out = 1;
          } else if (argv[i][j] == 'f' && argv[i][j + 1] != '\0') {
            add_pattern_from_file(&argv[i][j + 1], flags);
          } else if (argv[i][j] == 'e' && argv[i][j + 1] == '\0') {
            point_e = 1;
          } else if (argv[i][j] == 'f' && argv[i][j + 1] == '\0') {
            point_f = 1;
          }
        }
      } else {
        if (point_f) {
          add_pattern_from_file(argv[i], flags);
          point_f = 0;
        } else if (point_e) {
          add_pattern(argv[i], flags);
          point_e = 0;
        } else {
          add_file(argv[i], flags);
        }
      }
      out = 0;
    }
  } else {
    for (i = 1; i < argc; i++) {
      if (argv[i][0] != '-' && !was_there_a_pattern) {
        add_pattern(argv[i], flags);
        was_there_a_pattern = 1;
      } else if (argv[i][0] != '-') {
        add_file(argv[i], flags);
      }
    }
  }
}

int find_flags(char ch, opt *flags) {
  int res = 1;
  if (ch == 'e') {
    flags->e += 1;
    res = 0;
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
    res = 0;
  } else if (ch == 'o') {
    flags->o += 1;
  }
  return res;
}

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
        state = 3;
      }
    }
  }
  return state;
}

void free_memory(opt *flags) {
  for (long unsigned int i = 0; i < 2048; i++) {
    free(flags->files[i]);
  }
  free(flags->files);
  free(flags->patterns);
}

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

void add_file(char *filename, opt *flags) {
  strcpy(flags->files[flags->count_files], filename);
  flags->count_files += 1;
}

void make_pcre_patterns(opt *flags) {
  pcre *re = NULL;
  char str[1024] = {0};
  str[0] = '\0';
  int buffer[1024] = {0};
  int count = 0, string_num = 0;
  int c_flag_count = 0, out = 1;
  FILE *stream = NULL;
  re = compile_pattern(flags, flags->patterns);
  for (long unsigned int i = 0; i < flags->count_files; i++) {
    stream = fopen(flags->files[i], "r");
    if (stream) {
      while (!(feof(stream))) {
        string_num++;
        str[0] = '\0';
        fgets(str, sizeof(str), stream);
        count =
            pcre_exec(re, NULL, str, strlen(str), 0, 0, buffer, sizeof(buffer));
        if (count && strlen(str)) {
          if ((count > 0 && !(flags->v)) || (count < 0 && flags->v)) {
            if (flags->count_files > 1 && (!(flags->l) || flags->c) &&
                !(flags->h) && out) {
              printf("%s:", flags->files[i]);
            }
            if (flags->n && !(flags->l) && !(flags->c)) {
              printf("%d:", string_num);
            }
            if (flags->c) {
              out = 0;
              c_flag_count++;
            }
            if (flags->l) {
              if (flags->c) {
                printf("%d\n", c_flag_count);
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
              do_flag_o(flags, str);
            }
          }
        }
      }
      if (count && flags->c && (!(flags->l) || !c_flag_count)) {
        if (flags->count_files > 1 && !(flags->h) && c_flag_count == 0) {
          printf("%s:", flags->files[i]);
        }
        printf("%d\n", c_flag_count);
      }
      out = 1;
      c_flag_count = 0;
      fclose(stream);
    }
    string_num = 0;
  }
  if (re) {
    free(re);
  }
}

pcre *compile_pattern(opt *flags, char *pattername) {
  const char *error = NULL;
  int num_error = 0;
  pcre *re = NULL;
  if (flags->i) {
    re = pcre_compile(pattername, PCRE_CASELESS, &error, &num_error, NULL);
  } else {
    re = pcre_compile(pattername, 0, &error, &num_error, NULL);
  }
  if (!re) {
    free_memory(flags);
    free(re);
    exit(num_error);
  }
  return re;
}

void do_flag_o(opt *flags, char *str) {
  pcre *re = NULL;
  int count = 0;
  int buffer[1024] = {0};
  char *start_pos = NULL;
  int single_pattern_pos = 0;
  long unsigned int present_pos = 0;
  char single_pattern[1024] = {0};
  single_pattern[0] = '\0';
  start_pos = str;
  for (long unsigned int c = 0; c <= strlen(flags->patterns); c++) {
    if (flags->patterns[c] == '|' || flags->patterns[c] == '\0') {
      single_pattern[single_pattern_pos] = '\0';
      single_pattern_pos = 0;
      re = compile_pattern(flags, single_pattern);
      while ((strlen(start_pos) >= strlen(single_pattern)) &&
             (count = pcre_exec(re, NULL, start_pos, strlen(str), 0, 0, buffer,
                                sizeof(buffer))) > 0 &&
             (strlen(str) >= (present_pos + buffer[1] - 1))) {
        for (int i = buffer[0]; i < buffer[1]; i++) {
          putchar(start_pos[i]);
        }
        printf("\n");
        start_pos = &start_pos[buffer[1]];
        present_pos += buffer[1];
      }
      free(re);
    } else {
      single_pattern[single_pattern_pos++] = flags->patterns[c];
    }
  }
  start_pos = NULL;
}
