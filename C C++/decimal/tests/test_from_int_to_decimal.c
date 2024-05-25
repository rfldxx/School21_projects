#include <check.h>
#include <limits.h>
#include <stdio.h>

#include "s21_decimal.h"

static int src[] = {
    0,           -1,  1,    -32768,   32767,      212818281,   -2818281,
    -2147483647, 100, 9999, -9999999, 2147483647, -2147483648,
};

static s21_decimal dst[] = {
    {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},
    {{0x00008000, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00007FFF, 0x00000000, 0x00000000, 0x00000000}},
    {{0x0CAF5969, 0x00000000, 0x00000000, 0x00000000}},
    {{0x002B00E9, 0x00000000, 0x00000000, 0x80000000}},
    {{0x7FFFFFFF, 0x00000000, 0x00000000, 0x80000000}},
    {{0x00000064, 0x00000000, 0x00000000, 0x00000000}},
    {{0x0000270F, 0x00000000, 0x00000000, 0x00000000}},
    {{0x0098967F, 0x00000000, 0x00000000, 0x80000000}},
    {{0x7FFFFFFF, 0x00000000, 0x00000000, 0x00000000}},
    {{0x80000000, 0x00000000, 0x00000000, 0x80000000}},
};

START_TEST(test) {
  for (size_t i = 0; i < sizeof(src) / sizeof(int); ++i) {
    s21_decimal tmp;
    int ret = s21_from_int_to_decimal(src[i], &tmp);
    ck_assert_int_eq(tmp.bits[0], dst[i].bits[0]);
    ck_assert_int_eq(tmp.bits[1], dst[i].bits[1]);
    ck_assert_int_eq(tmp.bits[2], dst[i].bits[2]);
    ck_assert_int_eq(tmp.bits[3], dst[i].bits[3]);
    ck_assert_int_eq(ret, 0);
  }
}
END_TEST

Suite *Create_suite_s21_from_int_to_decimal() {
  Suite *suite = suite_create("from_int_to_decimal tests");
  TCase *tcase_core = tcase_create("Core");
  tcase_add_test(tcase_core, test);
  suite_add_tcase(suite, tcase_core);
  return suite;
}
