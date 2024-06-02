#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CUTEE
#define CUTEE 1
#endif

#define SEP ':'

//  -DL

#ifdef L
#define COMMANDS_LIST "benstv"
#define LONG_COMMANDS "-number-nonblank", "E", "-number", "-squeeze-blank", "T"
#if CUTEE == 0
#undef SEP
#define SEP '\t'
#endif
#else
#define COMMANDS_LIST "eivclnhsfo"
#endif

#define MAX_COUNT_OF_COMMANDS 10
const int N_COMMANDS = sizeof(COMMANDS_LIST) / sizeof(char) - 1;

#define cute_printf(space, info)         \
  if ((space) > 0) {                     \
    if (info != 0)                       \
      printf(_Generic(info, int          \
                      : "%*d%c", default \
                      : "%*s%c"),        \
             space, info, SEP);          \
    else if (CUTEE)                      \
      printf("%*c", space + 1, SEP);     \
  }

#define CreateMasiv(a, n, type) \
  type *a = (type *)malloc(((n) ? (n) : 1) * sizeof(type))

void update_max(int *a, int b) {
  if (b > *a) *a = b;
}
#define SIMULAR_HEAD                                                \
  CreateMasiv(files, argc, char *);                                 \
  CreateMasiv(files_type, argc, int);                               \
  bool commands[MAX_COUNT_OF_COMMANDS] = {0};                       \
  int files_count = parse(argc, argv, commands, files, files_type); \
  bool write_error = (N_COMMANDS < 8) | (!commands[7]);             \
  if (N_COMMANDS > 8 && re_gyve(files_count) > 0 &&                 \
      !(commands[0] | commands[8])) {                               \
    files_type[0] = 2;                                              \
    files_count--;                                                  \
  }                                                                 \
                                                                    \
  int longest_name = 0, digits_lines = 0, pattern_count = 0;        \
  for (int i = 0; files_count > 0 && files_type[i] != -1; i++)      \
    if (files_type[i] == 1) {                                       \
      update_max(&longest_name, strlen(files[i]));                  \
      update_max(&digits_lines, lines_n_digits(files[i], 1));       \
    } else if (files_type[i])                                       \
      pattern_count += files_type[i] == 3 ? lines_n_digits(files[i], 0) : 1;

#define FILES_ITERATION(start, something, ...)                          \
  {                                                                     \
    for (int i = start; files_type[i] != -1; i++)                       \
      if (files_type[i] == 1) {                                         \
        FILE *file = fopen(files[i], "r");                              \
        something(file, files[i], commands, longest_name, digits_lines, \
                  __VA_ARGS__);                                         \
        fclose(file);                                                   \
      } else if (write_error && !files_type[i])                         \
        printf("Problems with file: %s\n", files[i]);                   \
  }                                                                     \
  free(files);                                                          \
  free(files_type);

// what = 0 - кол-во строк в файле
// what = 1 - сколько разрадов нужно для этого числа
int lines_n_digits(char *filename, bool what) {
  FILE *f = fopen(filename, "r");
  int l = 1, d = 1;
  bool w = 0;
  char a;
  while (fscanf(f, "%c", &a) == 1)
    if (a == '\n') {
      l++;
      w = 0;
    } else
      w = 1;
  fclose(f);
  l -= !w;

  if (!what) return l;

  while (l /= 10) d++;
  return d;
}

bool is_file_open(char *name) {
  FILE *f = fopen(name, "r");
  if (f != NULL) {
    fclose(f);
    return 1;
  }
  return 0;
}

int puto(int i) { return i ? 1 << (8 * sizeof(int) - i) : 0; }
int re_gyve(int l) { return (l << 3) >> 3; }
void notify_troble(int l) {
  printf("We are lose the job:\n%s",
         !re_gyve(l) ? "\t• no file's to output\n" : "");
  printf("%s", l & puto(2) ? "\t• not exist flags\n" : "");
  printf("%s", l & puto(3) ? "\t• '-e' or '-f' at the end of input\n" : "");
}

// getopts

// a.out -e dcscd -ef mads
//  files_type:
//    0 - файл не открыввается
//    1 - это файл который надо будет "напечатать"
//    2 - это строка-патерн для grep-а               '-e'
//    3 - это файл с патернами для grep-а            '-f'
//   -1 - файлы закончились
//  функция возвращает:  is_correct_parse * count_file_to_print
int parse(int n, char **input, bool *commands, char **files, int *files_type) {
  const char list[] = COMMANDS_LIST;
#ifdef L
  const char *long_commands[] = {LONG_COMMANDS};
#endif
  bool is_corect = 1;
  int wf = 0, files_count = 0;  // wf - бегунок files_type

  for (int i = 1; i < n; i++) {
    if (input[i][0] == '-') {
#ifdef L
      if (input[i][1] == '-' || input[i][1] == 'E' || input[i][1] == 'T')
        for (int j = 0; j < N_COMMANDS; j++) {
          if (!strcmp(input[i] + 1, long_commands[j])) {
            commands[j] = 1;
            break;
          }
          is_corect &= (j < N_COMMANDS - 1);
        }
      else
#endif
        for (char *pos = input[i] + 1; *pos != '\0'; pos++) {
          for (int j = 0; j < N_COMMANDS; j++) {
            if (list[j] == *pos) {
#ifndef L
              //если это 'e' или 'f' в grep то надо запоминать шаблон и выходить
              if (list[j] == 'e' || list[j] == 'f') {
                files[wf] =
                    *(pos + 1)
                        ? pos + 1
                        : input[++i < n ? i : 0];  //а что если мы передали:
                                                   //|grep example.txt -e| и всё
                if (i == n)
                  return puto(1) | puto(2 * !is_corect) | puto(3) |
                         files_count;  //этот флаг ('-e' или '-f') в конце ввода
                                       //- форсировано вырубаем всё
                files_type[wf] =
                    (list[j] == 'e') ? 2 : 3 * is_file_open(files[wf]);
                wf++;
                *(pos--) = '\0';  //вместо этого "флага" ставим конец строки и
                                  //из-за этого вырубиться for(pos)
              }
#else
            commands[5] |=
                (list[j] == 'e') ||
                (list[j] ==
                 't');  //если это '-e' или '-t' в cat, то надо включить '-v'
#endif
              commands[j] = 1;
              break;
            }
            is_corect &= (j < N_COMMANDS - 1);
          }
        }
    } else {
      files[wf] = input[i];
      files_type[wf] = is_file_open(files[wf]);
      files_count++;
      wf++;
    }
  }
  files_type[wf] = -1;

  return puto(!is_corect) | puto(2 * !is_corect) | files_count;
}
