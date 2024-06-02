#include "../common/both_stuff.h"

// keep_empty - мега искуственная переменая, нужна только для того чтобы макрос FILES_ITERATION нормально работал
void print_file(FILE *file, char *filename, bool *commands, int longest_name, int digits_lines, int keep_empty) {
  int l = 0;
  bool new_line = 1;
  digits_lines *= commands[0] || commands[2];

  int squeeze = 0;

  char scroller;
  while (fscanf(file, "%c", &scroller) == 1) {
    squeeze = (scroller == '\n' && commands[3] && new_line) * (squeeze + 1);

    if (new_line && squeeze < keep_empty + 1) {
      cute_printf(longest_name, filename);
      int use = commands[0] && scroller == '\n' ? 0 : ++l;
      cute_printf(digits_lines, use);
    }

    new_line = (scroller == '\n');

    if (squeeze <= keep_empty) {  //от подряд идущих пустых строк остается не более keep_empty штук
           if (scroller == '\n')               printf("%s\n", commands[1] ?  "$" :   "");
      else if (scroller == '\t')               printf("%s"  , commands[4] ? "^I" : "\t");
      else if (commands[5] && scroller  <  32) printf("^%c" , '@' + scroller);
      else if (commands[5] && scroller == 127) printf("^?");
      else                                     printf("%c", scroller);
    }
  }
}

int main(int argc, char *argv[]) {
  SIMULAR_HEAD;

  longest_name *=  CUTEE * (files_count >  1);
  digits_lines += !CUTEE * (6 - digits_lines);

  if (files_count <= 0) notify_troble(files_count);
  else                  FILES_ITERATION(0, print_file, 1);
}
