#include <limits.h>
#include <regex.h>

#include "../common/both_stuff.h"

bool verdict(char *line, int *widespread, int pattern_count, regex_t *eryndushka, regmatch_t *bounds, int *absolute_pos, bool is_first) {
  int next_widespread = INT_MAX;

  for (int i = 0; i < pattern_count; i++) {
    if (is_first || absolute_pos[i] == *widespread) {
      if (is_first) absolute_pos[i]  = 0;
      else          absolute_pos[i] += (bounds[i].rm_eo - bounds[i].rm_so);

      if (!regexec(eryndushka + i, line + absolute_pos[i], 1, bounds + i, 0)) {
        absolute_pos[i] += bounds[i].rm_so;
      } else {
        absolute_pos[i]  = INT_MAX;
      }
    }

    if (next_widespread > absolute_pos[i]) { next_widespread = absolute_pos[i]; }
  }
  *widespread = next_widespread;

  return next_widespread != INT_MAX;
}

void clever_print(FILE *file, char *filename, bool *commands, int longest_name, int digits_lines, int pattern_count, regex_t *eryndushka, bool ultra) {
  char *line = NULL;
  size_t capacity = 0;

  // cute_offset используется только с CUTEE
  int cute_offset = (digits_lines + (digits_lines > 0)) + (!commands[6] && longest_name ? abs(longest_name) + 1 : 0);
  int l, matched_line = 0, current_line = 0, widespread;

  CreateMasiv(bounds, pattern_count, regmatch_t);
  CreateMasiv(absolute_pos, pattern_count,  int);

  while ((int)(l = getline(&line, &capacity, file)) != -1) {
    current_line++;

    // 2 - значит ultra ветка
    int branch = ultra ? 2 * !commands[2]
                       : (pattern_count || CUTEE) && (commands[2] ^ verdict(line, &widespread, pattern_count, eryndushka, bounds, absolute_pos, 1));
    //такой кринж, потому что если передано 0 шаблонов то grep во всех случаях ничго не выводит

    if (branch) {
      matched_line++;
      if (commands[3] || commands[4]) continue;

      cute_printf(longest_name,     filename);
      cute_printf(digits_lines, current_line);
      if (ultra || !(commands[9] && !commands[2])) printf("%s%s", !ultra || !commands[2] ? line : "", line[l - 1] != '\n' ? "\n" : "");
    }

    if (branch == 1 && commands[9] && !commands[2]) {
      int need_ofset = 0;

      do {
        for (int i = 0; i < pattern_count; i++) {
          if (bounds[i].rm_so != -1 && absolute_pos[i] == widespread) {
            int volume = (int)(bounds[i].rm_eo - bounds[i].rm_so);
            printf("%*.*s\n", (need_ofset++ && CUTEE) * (cute_offset + volume), volume, line + absolute_pos[i]);
          }
        }
      } while (verdict(line, &widespread, pattern_count, eryndushka, bounds, absolute_pos, 0));
    }
  }

  free(line);
  free(absolute_pos);
  free(bounds);

  if (commands[3] && matched_line >= 0) {
    cute_printf(longest_name, filename);
    printf("%d\n", commands[4] ? matched_line > 0 : matched_line);
  }
  if (commands[4] && matched_line) printf("%*s\n", CUTEE * abs(longest_name), filename);
}

int main(int argc, char *argv[]) {
  SIMULAR_HEAD;
  // if (CUTEE) printf("pattern count = %d\n", pattern_count);

  bool ultra       = 0;  //если паттерном подали пустую строку
  bool bad_pattern = 0;  // не скомпилированый патерн

  CreateMasiv(eryndushka, pattern_count, regex_t);
  size_t pattern_capacity = 0, l;
  char *pattern_line = NULL;
  for (int i = 0, reg_pos = 0; files_count > 0 && files_type[i] != -1; i++) {
    if (files_type[i] == 3) {
      FILE *file = fopen(files[i], "r");
      while ((int)(l = getline(&pattern_line, &pattern_capacity, file)) != -1) {
        if (l && pattern_line[0] != '\n') {
          if (pattern_line[l - 1] == '\n') pattern_line[l - 1] = '\0';

          if (regcomp(eryndushka + (reg_pos++), pattern_line, commands[1] * REG_ICASE)) {
            reg_pos--;
            bad_pattern = 1;
          }
        } else {
          ultra = 1;  //если ultra стала равна 1, то pattern_count дальше не используется в программе
        }  
      }
      fclose(file);
    } else if (files_type[i] == 2) {
      regcomp(eryndushka + (reg_pos++), files[i], commands[1] * REG_ICASE);
    }

    if (files_type[i + 1] == -1) pattern_count = reg_pos;
  }

  if (pattern_line) free(pattern_line);

  digits_lines = (CUTEE ? digits_lines : 1) * commands[5];
  longest_name = (CUTEE ? longest_name : 1) * (2 * (!commands[6] && re_gyve(files_count) > 1) - 1);

  if (CUTEE) printf("pattern count = %d\n", pattern_count);

  if (files_count <= 0 || (bad_pattern && !(CUTEE && commands[7]))) {
    notify_troble(files_count);
    if (bad_pattern) printf("\t•regcomp can't process one of ptterns\n");
  } else
    FILES_ITERATION(0, clever_print, pattern_count, eryndushka, ultra);

  for (int i = 0; i < pattern_count; i++) regfree(eryndushka + i);
  free(eryndushka);
  return 0;
}
