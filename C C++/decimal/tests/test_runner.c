#include <check.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Suite *Create_suite_s21_add();
Suite *Create_suite_s21_sub();
Suite *Create_suite_s21_mul();
Suite *Create_suite_s21_div();

Suite *Create_suite_s21_is_less();
Suite *Create_suite_s21_is_less_or_equal();
Suite *Create_suite_s21_is_greater();
Suite *Create_suite_s21_is_greater_or_equal();
Suite *Create_suite_s21_is_equal();
Suite *Create_suite_s21_is_not_equal();

Suite *Create_suite_s21_from_decimal_to_int();
Suite *Create_suite_s21_from_decimal_to_float();
Suite *Create_suite_s21_from_float_to_decimal();
Suite *Create_suite_s21_from_int_to_decimal();

Suite *Create_suite_s21_floor();
Suite *Create_suite_s21_negate();
Suite *Create_suite_s21_truncate();
Suite *Create_suite_s21_round();

#define Same_figna(name)                                                \
  Suite *suite_##name = Create_suite_s21_##name();                      \
  SRunner *suite_runner_##name = srunner_create(suite_##name);          \
  srunner_run_all(suite_runner_##name, CK_NORMAL);                      \
  int failed_count_##name = srunner_ntests_failed(suite_runner_##name); \
  srunner_free(suite_runner_##name);                                    \
  if (failed_count_##name != 0) {                                       \
    printf("Failed tests: %d\n", failed_count_##name);                  \
    exit_code = EXIT_FAILURE;                                           \
  } else                                                                \
    printf("Success\n");                                                \
  printf("\n");

int main(void) {
  printf("\n");
  bool exit_code = EXIT_SUCCESS;

  Same_figna(add);
  Same_figna(sub);
  Same_figna(mul);
  Same_figna(div);

  Same_figna(is_less);
  Same_figna(is_less_or_equal);
  Same_figna(is_greater);
  Same_figna(is_greater_or_equal);
  Same_figna(is_equal);
  Same_figna(is_not_equal);

  Same_figna(from_decimal_to_int);
  Same_figna(from_decimal_to_float);
  Same_figna(from_float_to_decimal);
  Same_figna(from_int_to_decimal);

  Same_figna(floor);
  Same_figna(negate);
  Same_figna(truncate);
  Same_figna(round);

  return exit_code;
}

