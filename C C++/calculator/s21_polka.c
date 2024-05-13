#include "s21_polka.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MULTIPLY -----------------------------------------
#define FIRST(a1, ...) a1
#define SECND(a1, a2, ...) a2
#define THIRD(a1, a2, a3, ...) a3
#define wFIRST(...) FIRST(__VA_ARGS__)
#define wTHIRD(...) THIRD(__VA_ARGS__)

#define SAME_START(i) r##i( r##i( r##i( r##i(
#define r1(...) __VA_ARGS__
#define r2(...) SAME_START(1) __VA_ARGS__ ))))
#define r3(...) SAME_START(2) __VA_ARGS__ ))))
#define r(...) r3(__VA_ARGS__)

#define detect0(...) ~, ~
#define EXIST_ARG(...) wTHIRD(wFIRST(detect0 detect0 __VA_ARGS__)(), 0, 1)

#define SKIP(...)
#define OK(...) __VA_ARGS__
#define cat(a, b) a##b
#define IF(t) cat(IF, t)
#define IF0(...) OK
#define IF1(...) __VA_ARGS__ SKIP

#define FOLLOWER(F, CH, a, b, ...) \
  (EXIST_ARG(__VA_ARGS__))(F, CH, CH(a, b), __VA_ARGS__)
#define p_(x) IF(x)(p)(SKIP)
#define p(F, CH, a, b, ...) F(a, b) p_ FOLLOWER(F, CH, a, b, __VA_ARGS__)

#define MULTIPLY_2gen(F, CH, a, ...) \
  IF(EXIST_ARG(__VA_ARGS__))(r(p(F, CH, a, __VA_ARGS__)))()
// --------------------------------------------------
const int zero_error = 0;
#define NEXT(a, b) const int b = a - 1;
#define STR(skip, a) #a,

#define SET_ERROR(...)                                \
  MULTIPLY_2gen(NEXT, SECND, zero_error, __VA_ARGS__) \
  const char text_error[][100] = { MULTIPLY_2gen(STR, SECND, 0, __VA_ARGS__) "LAZY"};
// --------------------------------------------------

SET_ERROR(SO_MANY_DOTS_err, UNARI_err, UNKNOWN_OPEARATION_err,
          UNKNOWN_SYMBOL_err, EMPTY_BRACKETS_err,
          NO_VALID_ARGUMENT_FOR_UNARY_err, NO_VALID_ARGUMENT_FOR_OPERATIONS_err,
          NO_VALID_ARGUMENT_FOR_FUNCTION_err, NUMBERS_IN_A_ROW_err,
          OPERATIONS_IN_A_ROW_err, BRACKET_err, DIFFERENCE_BRACKET_err)

// #define       SO_MANY_DOTS_err  -1
// #define              UNARI_err  -2
// #define UNKNOWN_OPEARATION_err  -3
// #define     UNKNOWN_SYMBOL_err  -4

#define NAMEx(space, f, name, ...) { name, f }
#define NAME(a, ...) NAMEx(, ##__VA_ARGS__, a, #a)

struct func_t {
  char name[20];
  double (*f)(double);
};
typedef struct func_t func;

double unari_u(double x) { return -x; }
func functions[] = {NAME(cos),          NAME(sin),        NAME(tan),
                    NAME(acos),         NAME(asin),       NAME(atan),
                    NAME(sqrt),         NAME("ln", log),  NAME("log", log10),
                    NAME("-", unari_u), NAME("abs", fabs)};
// function quantity
const int fq = sizeof(functions) / sizeof(*functions);

const char bracket[] = "()|";
const char delimetr[] = "+-*/^%%";
const char space[] = " \t\n";

// const char operations[] = "^*/+-";

int N = 0;
info parts[maxLixem] = {0};

int isDigit(char a, int* dot_count, int change) {
  if (change) *dot_count += a == '.';
  return ('0' <= a && a <= '9') || a == '.';
}
int isLeter(char a) { return ('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z'); }

char save_abs[] = "abs";
void serve(const char* dst, char* data_dst) {
  N = 0;

  for (int i = 0; *dst; N++) {
    while (*dst && strchr(space, *dst)) dst++;
    if (!*dst) break;
    parts[N].text = data_dst + i;

    // число (комбинация цифр и точек)
    parts[N].mode = 3;
    if (isDigit(*dst, &(parts[N].mode), 0)) {
      int l = i;
      while (isDigit(*dst, &(parts[N].mode), 1)) data_dst[i++] = *dst++;
      if (parts[N].mode == 4 && (i - l == 1))  // это одна точка
        parts[N].mode = SO_MANY_DOTS_err;
    }

    // скобки
    else if (strchr(bracket, *dst)) {
      parts[N].mode = 1;
      data_dst[i++] = *dst++;
    }

    // операция
    else if (strchr(delimetr, *dst)) {
      parts[N].mode = 2;
      data_dst[i++] = *dst++;
    }

    // слово
    else if (isLeter(*dst)) {
      parts[N].mode = 0;
      while (isLeter(*dst)) data_dst[i++] = *dst++;
    }

    // не обрабатываемое
    else {
      data_dst[i++] = *dst++;
      parts[N].mode = UNKNOWN_SYMBOL_err;
    }

    data_dst[i++] = 0;
  }
}

// возможно стоит добавить в эту функцию проверку на то, что это не код ошибки
int isi(int i, char a) {
  char buff[2] = {a};
  return !strcmp(parts[i].text, buff);
}

int curr_arg_quantity[maxLixem] = {0};
void count_args() {
  for (int i = 0, q = 0; i < N; i++) {
    if (parts[i].mode == 2) q--;
    else if (parts[i].mode >= 3 || parts[i].mode == SO_MANY_DOTS_err) q++;

    curr_arg_quantity[i] = q;
  }
}

void rails() {
  for (int i = 0; i < N; i++) {
    if (!strcmp(parts[i].text, "mod")) {
      parts[i].mode = 2;
      parts[i].text[0] = '%';
      parts[i].text[1] = 0;
      continue;
    }

    if (isi(i, 'x')) { parts[i].mode = 5; continue; }

    if (parts[i].mode >= 5) { parts[i].mode = SO_MANY_DOTS_err; continue; }

    if (parts[i].mode == 0) {  // проверяем "слова" (mode == 0)
      parts[i].mode = UNKNOWN_OPEARATION_err;
      for (int j = 0; j < fq && parts[i].mode; j++)
        if (!strcmp(parts[i].text, functions[j].name)) parts[i].mode = 0;
    }
  }

  // поиск унарных операций
  // попутно заполняя curr_arg_quantity
  for (int i = 0, q = 0; i < N; i++) {
    int isUnary = isi(i, '-') || isi(i, '+');

    if (i != 0 && isUnary) {
      int prev_is_close_bracket = (curr_arg_quantity[i - 1] != 0 && isi(i - 1, '|')) || isi(i - 1, ')');
      int prev_is_number = parts[i - 1].mode >= 3 || parts[i].mode == SO_MANY_DOTS_err;

      if (prev_is_close_bracket || prev_is_number) isUnary = 0;
    }

    if (isUnary) parts[i].mode = 0;

    if (parts[i].mode == 2) q--;
    else if (parts[i].mode >= 3 || parts[i].mode == SO_MANY_DOTS_err) q++;

    curr_arg_quantity[i] = q;
  }

  // отмечаем ошибки вида f --x    a * --x
  //                        ^          ^
  for (int i = 0; i < N; i++) {
    if (!isi(i, '+') && !isi(i, '-')) continue;

    int breaks = 0;
    if (i != 0) {
      if (parts[i - 1].mode == 0 || parts[i - 1].mode == 2) breaks = 1;
    }

    if (breaks && i + 1 < N && (isi(i + 1, '+') || isi(i + 1, '-')))
      parts[i].mode = UNARI_err;

    while (i < N && (isi(i, '+') || isi(i, '-'))) i++;
  }

  // схлопываем + - (все знаки кроме последнего)
  // +--+-+-+--  =  [+--+-+-+-]-  =  - -
  // +--+-+-+-+  =  [+--+-+-+-]+  =  - +  =  -
  for (int i = 0; i < N; i++) {
    if (parts[i].mode < 0 || (!isi(i, '+') && !isi(i, '-'))) continue;
    // нашли + -

    int sign = 1, j = i;
    for (; j < N && parts[j].mode >= 0 && (isi(j, '+') || isi(j, '-')); j++)
      if (isi(j, '-')) sign *= -1;

    int shift = j - i - (sign == -1 || parts[i].mode == 2);
    if (!shift) continue;

    parts[i].text[0] = sign == 1 ? '+' : '-';
    for (int t = j; t < N; t++) parts[t - shift] = parts[t];
    N -= shift;
  }
}

char* color(char* err_buff, int space, const char* s, int clr) {
  err_buff += sprintf(err_buff, "<font color=\"");

  if (clr == 0) err_buff += sprintf(err_buff, "red");
  if (clr == 1) err_buff += sprintf(err_buff, "maroon");
  if (clr == 2) err_buff += sprintf(err_buff, "green");

  err_buff += sprintf(err_buff, "\">%*s</font>", (int)(space + strlen(s)), s);
  return err_buff;
}

#define ENTER "<br>"
int show_error(char* err_buff) {
  const int max_count_error = 5;

  int count = 0;
  int err_pos[maxLixem] = {0};
  int err_ind[maxLixem];

  for (int i = 0; i < N; i++) {
    if (parts[i].mode < 0) {
      err_ind[count] = i;
      count++;
    }
    err_pos[count] += strlen(parts[i].text) + 1;
  }
  if (!count) return 0;

  // сообщаем не более чем о max_count_error ошибок
  if (count > max_count_error) count = max_count_error;

  err_buff = color(err_buff, 0, "ERROR:" ENTER, 0);
  for (int i = 0, t = 0; i < N; i++, err_buff += sprintf(err_buff, " ")) {
    int gg = parts[i].mode == 0 && (isi(i, '-') || isi(i, '+'));
    if (gg) parts[i].text[0] = 'u';

    if (t >= count || i != err_ind[t]) err_buff += sprintf(err_buff, "%s", parts[i].text);
    else                               err_buff  = color(err_buff, 0, parts[i].text, (t++) % 2);

    if (gg) parts[i].text[0] = '-';
  }
  err_buff += sprintf(err_buff, ENTER);

  for (int t = 0; t < count; t++) err_buff = color(err_buff, err_pos[t] - (t != 0), "^", t % 2);
  err_buff += sprintf(err_buff, ENTER);

  for (int l = 0; l < count; l++) {
    int t = 0;
    for (; t < count - l - 1; t++) err_buff = color(err_buff, err_pos[t] - (t != 0), "|", t % 2);
    err_buff  = color(err_buff, err_pos[t] - (t != 0), text_error[-parts[err_ind[t]].mode - 1], t % 2);
    err_buff += sprintf(err_buff, ENTER);
  }
  *err_buff = 0;

  return 1;
}

struct binary_t {
  char name;
  int priory;
  int inv;  // раскрываются слева:  a^b^c = a^(b^c)
  double (*f)(double, double);
};
typedef struct binary_t binary;

// --------------------------------------------------
#define NFRST(a1, ...) __VA_ARGS__
#define GET(name, wrap_b) wwNEW_OP(name, NFRST wrap_b)
#define wwNEW_OP(...) wNEW_OP(__VA_ARGS__)
#define wNEW_OP(name, c, ...) oNEW_OP(name, c, FIRST(__VA_ARGS__, ))
#define oNEW_OP(...) NEW_OP(__VA_ARGS__)

#define NEW_OP(name, c, ...)    \
double name(double a, double b) { return IF(EXIST_ARG(__VA_ARGS__))(__VA_ARGS__(a, b))(a c b); }

#define gCHAR(a) #a[0]
#define wgCHAR(...) gCHAR(__VA_ARGS__)
#define INVV(a, b, c, d, ...) d
#define wINVV(...) INVV(__VA_ARGS__, 0, 0, 0, 0)
#define LNAME(name, wrap_b) {wgCHAR(SECND wrap_b), FIRST wrap_b, wINVV wrap_b, name},

#define opium(a, ...) cat(a, I)
#define SET_OPERATIONS(...)                     \
  MULTIPLY_2gen(GET, opium, opI, __VA_ARGS__)   \
  const binary operations[] = { MULTIPLY_2gen(LNAME, opium, opI, __VA_ARGS__){0} };
// --------------------------------------------------

// const char delimetr[] = "+-%*/^";
SET_OPERATIONS((0, +), (0, -), (1, %, fmod), (2, *), (2, /), (7, ^, pow, 1))

const int function_priory = 5;
const int unarymin_priory = 10;

int get_ind(info op) {
  for (int i = 0; i < (int)(sizeof(operations) / sizeof(*operations)) - 1; i++)
    if (operations[i].name == op.text[0]) return i;
  return (sizeof(operations) / sizeof(*operations)) - 1;
}

int op_priory(info op) { return operations[get_ind(op)].priory; }
int op_is_inv(info op) { return operations[get_ind(op)].inv; }

info stlack[maxLixem];
int jstlack = 0;  // в stlack

const info abs_val = {"abs", 0, -1};
int Deiksta(info* result, int* Nresult, char* err_buff) {
  for (int i = 0; i < N; i++) parts[i].pos = i;
  count_args();
  info* write = result;
  jstlack = 0;

  for (int i = 0; i < N; i++) {
    if (parts[i].mode >= 3) {  //  число
      *write++ = parts[i];
      continue;
    }

    if (parts[i].mode != 1) {  // операция (надеюсь не код ошибки)
      int curr_priory = parts[i].mode != 2 ? function_priory : op_priory(parts[i]);

      if (parts[i].mode == 0 && isi(i, '-')) curr_priory = unarymin_priory;

      int is_curr_inv = parts[i].mode != 2 ? 1 : op_is_inv(parts[i]);

      int prev_priory, repeat;

      if (parts[i].mode != 0) do {
          prev_priory = -1;
          if (jstlack != 0) {
                if (stlack[jstlack - 1].mode == 2) prev_priory = op_priory(stlack[jstlack - 1]);
           else if (stlack[jstlack - 1].mode == 0) prev_priory = stlack[jstlack - 1].text[0] == '-' ? unarymin_priory : function_priory;
          }

          if ((repeat = prev_priory > curr_priory || (prev_priory == curr_priory && !is_curr_inv))) {
            *write++ = stlack[--jstlack];
          }
        } while (repeat);

      stlack[jstlack++] = parts[i];
      continue;
    }

    // скобки
    if (isi(i, '(') || (isi(i, '|') && curr_arg_quantity[i] == 0)) {  // открывающая
      stlack[jstlack++] = parts[i];
      continue;
    }

    // закравающаяся
    int j = jstlack - 1;
    while (0 <= j && stlack[j].mode != 1) *write++ = stlack[j--];

    int mismatch = 0;
    if (0 <= j && (isi(i, ')') ? stlack[j].text[0] != '(' : stlack[j].text[0] != '|')) {
      parts[stlack[j].pos].mode = DIFFERENCE_BRACKET_err;
      mismatch = 1;
    }
    if (j < 0 || mismatch) {
      parts[i].mode = BRACKET_err;

      show_error(err_buff);
      return 0;
    }
    j--;

    // добавляем abs после |..|
    int is_prev_abs = write == result ? 0 : !strcmp(write[-1].text, abs_val.text);
    if (isi(i, '|') && !is_prev_abs) {
      *write = abs_val;
      write->pos = stlack[j + 1].pos;
      write++;
    }

    while (0 <= j && stlack[j].mode == 0) *write++ = stlack[j--];
    jstlack = j + 1;
  }

  int j = jstlack - 1;
  while (0 <= j) {
    if (stlack[j].mode == 1) {
      parts[stlack[j].pos].mode = BRACKET_err;
      show_error(err_buff);
      return 0;
    }

    *write++ = stlack[j--];
  }

  *Nresult = write - result;

  return 1;
}

int max_temp_count = 0;
int omaru_test(info* result, int Nresult, char* err_buff) {
  max_temp_count = 0;
  int M = Nresult;
  info test_result[maxLixem];
  for (int i = 0; i < Nresult; i++) test_result[i] = result[i];

  int need_temp_count = 0;
  for (int pos = 0; pos < M; pos++) {
    if (test_result[pos].mode == 0) {
      if (pos < 1 || test_result[pos - 1].pos < test_result[pos].pos) {
        parts[test_result[pos].pos].mode = test_result[pos].text[0] == '-' ? NO_VALID_ARGUMENT_FOR_UNARY_err : NO_VALID_ARGUMENT_FOR_FUNCTION_err;
        show_error(err_buff);
        return 0;
      }

      // сдвигаем всЁ
      for (int j = pos; j < M; j++) test_result[j - 1] = test_result[j];
      M--;
      pos--;
    } else if (test_result[pos].mode == 2) {
      need_temp_count--;

      if (pos < 2 || !(test_result[pos - 2].pos < test_result[pos].pos && test_result[pos].pos < test_result[pos - 1].pos)) {
        parts[test_result[pos].pos].mode = NO_VALID_ARGUMENT_FOR_OPERATIONS_err;
        show_error(err_buff);
        return 0;
      }

      // сдвигаем всЁ
      test_result[pos].pos = test_result[pos - 2].pos;
      for (int j = pos; j < M; j++) test_result[j - 2] = test_result[j];
      M   -= 2;
      pos -= 2;
    } else if (test_result[pos].mode >= 3)  need_temp_count++;

    if (need_temp_count > max_temp_count) max_temp_count = need_temp_count;
  }

  if (M > 1) {
    for (int i = 0; i < M - 1; i++)
      parts[test_result[i].pos].mode = NUMBERS_IN_A_ROW_err;

    show_error(err_buff);
    return 0;
  } else if (M < 1) {
    err_buff = color(err_buff, 0, "ERROR:" ENTER, 0);
    strcpy(err_buff, "No valid args");
    return 0;
  }

  return 1;
}

double nothing(double a) { return a; }

double (*get_function(info op))(double) {
  for (int i = 0; i < fq; i++)
    if (!strcmp(functions[i].name, op.text)) return functions[i].f;
  return nothing;
}

double* calculate_graph(const info* result, int Nresult, int last_pls, const int n, const double* x_val) {
  double** section = malloc(last_pls * sizeof(double*));

  for (int i = 0; i < last_pls; i++) section[i] = malloc(n * sizeof(double));

  int q = 0;
  for (int i = 0; i < Nresult; i++) {
    if (result[i].mode == 5) {
      for (int j = 0; j < n; j++) section[q][j] = x_val[j];
      q++;
    } else if (result[i].mode >= 3) {
      double val = atof(result[i].text);
      for (int j = 0; j < n; j++) section[q][j] = val;
      q++;
    }

    else if (result[i].mode == 0) {
      double (*function)(double) = get_function(result[i]);
      for (int j = 0; j < n; j++)
        section[q - 1][j] = function(section[q - 1][j]);
    } else if (result[i].mode == 2) {
      double (*operator)(double, double) = operations[get_ind(result[i])].f;
      for (int j = 0; j < n; j++)
        section[q - 2][j] = operator(section[q - 2][j], section[q - 1][j]);
      q -= 1;
    }
  }

  double* data = section[0];
  for (int i = 1; i < last_pls; i++) free(section[i]);
  free(section);
  return data;
}

int omaru_polka(const char* input, char* data_dst, info* result, int* Nresult, int* last_pls, char* log_info) {
  max_temp_count = 0;

  int temp_N = 0;
  char temp_dst[2 * maxLixem];
  serve(input, temp_dst);
  rails();

  if (show_error(log_info)) return 0;
  if (!Deiksta(result, &temp_N, log_info)) return 0;
  if (!omaru_test(result, temp_N, log_info)) return 0;

  for (int i = 0; i < 2 * maxLixem; i++) data_dst[i] = temp_dst[i];
  for (int i = 0; i < temp_N; i++)
    if (strcmp("abs", result[i].text))
      result[i].text = data_dst + (result[i].text - temp_dst);

  *Nresult = temp_N;
  *last_pls = max_temp_count;

  write_polka(result, *Nresult, log_info);

  return max_temp_count;
}

void write_polka(const info* result, int Nresult, char* log_info) {
  log_info = color(log_info, 0, "POLKA:", 2);
  log_info += sprintf(log_info, " (need temp double's - %d)" ENTER, max_temp_count);
  for (int i = 0; i < Nresult; i++) {
    if (result[i].mode == 0 && result[i].text[0] == '-') log_info += sprintf(log_info, "u ");
    else                                                 log_info += sprintf(log_info, "%s ", result[i].text);
  }
  *log_info = 0;
}

int exist_x(const info* result, int Nresult) {
  while (Nresult--)
    if ((result ++)->mode == 5) return 1;
  return 0;
}
