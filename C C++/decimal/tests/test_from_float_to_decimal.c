#include <check.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

#include "check.h"

static float src[] = {
    0.f,
    1E-28f,
    1.23699997E-25f,
    4.91059997E-24f,
    1.1E-17f,
    1.10000001E-14f,
    0.109999999f,
    2.38182998f,
    -0.f,
    -1.f,
    1.f,
    -2.f,
    2.f,
    -32768.f,
    32767.f,
    -9999999.f,
    1E-28f,
};

static s21_decimal dst[] = {
    {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x001C0000}},
    {{0x000004D5, 0x00000000, 0x00000000, 0x001C0000}},
    {{0x0000BFD2, 0x00000000, 0x00000000, 0x001C0000}},
    {{0x0000000B, 0x00000000, 0x00000000, 0x00120000}},
    {{0x0000000B, 0x00000000, 0x00000000, 0x000F0000}},
    {{0x0000000B, 0x00000000, 0x00000000, 0x00020000}},
    {{0x0003A267, 0x00000000, 0x00000000, 0x00050000}},
    {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
    {{0x00000002, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00000002, 0x00000000, 0x00000000, 0x00000000}},
    {{0x00008000, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00007FFF, 0x00000000, 0x00000000, 0x00000000}},
    {{0x0098967F, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x001C0000}},
};

START_TEST(from_float_to_decimal_test) {
  for (size_t i = 0; i < sizeof(src) / sizeof(float); ++i) {
//      printf("%d\n", i);
      
    s21_decimal tmp;
    int ret = s21_from_float_to_decimal(src[i], &tmp);
      check_decimal(tmp, dst[i]);
    ck_assert_int_eq(ret, 0);
  }
}
END_TEST

Suite *Create_suite_s21_from_float_to_decimal() {
  Suite *suite = suite_create("from_float_to_decimal tests");
  TCase *tcase_core = tcase_create("Core");
  tcase_add_test(tcase_core, from_float_to_decimal_test);
  suite_add_tcase(suite, tcase_core);
  return suite;
}
