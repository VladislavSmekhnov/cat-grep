#ifndef SRC_GREP_MAIN_H_
#define SRC_GREP_MAIN_H_

#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  size_t files_number;
  int pattern_length;
  char **files;
  char *patterns;
} opt;

void sort_bash_first(int argc, char *argv[], opt *flags);
void sort_bash_second(int argc, char *argv[], opt *flags);
int collect_flags(char ch, opt *flags);
int get_memory(opt *flags);
void free_memory(opt *flags);
void add_pattern_from_file(char *filename, opt *flags);
void add_pattern(char *patternname, opt *flags);
void add_file(char *filename, opt *flags);
void make_pcre_patterns(opt *flags);
pcre *create_pattern(opt *flags, char *patternname);
void run_flag_o(opt *flags, char *patternname);

#endif  // SRC_GREP_MAIN_H_