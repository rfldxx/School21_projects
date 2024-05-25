#include <check.h>
#include <limits.h>
#include <stdio.h>

#include "s21_decimal.h"

static s21_decimal arr[] = {
    {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
    {{0xCF5C048C, 0x004D2BEF, 0x00000000, 0x00070000}},
    {{0xFEADBDA2, 0x000001FB, 0x00000000, 0x80060000}},
    {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}},
    {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}},
    {{0x00000000, 0x00000000, 0x00000000, 0x00020000}},
    {{0x00000467, 0x0000007A, 0x00000000, 0x00040000}},
    {{0x00000467, 0x0000007A, 0x00000000, 0x80000000}},
    {{0x00000467, 0x0000007A, 0x00003215, 0x00010000}},
    {{0x00000467, 0x0000007A, 0x00205FF0, 0x80020000}},
    {{0x000F4240, 0x00002710, 0x00002710, 0x80050000}},
};

static s21_decimal result[] = {
    {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},
    {{0xCF5C048C, 0x004D2BEF, 0x00000000, 0x80070000}},
    {{0xFEADBDA2, 0x000001FB, 0x00000000, 0x00060000}},
    {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}},
    {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}},
    {{0x00000000, 0x00000000, 0x00000000, 0x80020000}},
    {{0x00000467, 0x0000007A, 0x00000000, 0x80040000}},
    {{0x00000467, 0x0000007A, 0x00000000, 0x00000000}},
    {{0x00000467, 0x0000007A, 0x00003215, 0x80010000}},
    {{0x00000467, 0x0000007A, 0x00205FF0, 0x00020000}},
    {{0x000F4240, 0x00002710, 0x00002710, 0x00050000}},
};

START_TEST(test) {
  for (size_t i = 0; i < sizeof(arr) / sizeof(s21_decimal); ++i) {
    s21_decimal value = arr[i];
    s21_decimal tmp;
    int ret = s21_negate(value, &tmp);
    for (int j = 0; j < 4; j++)
      ck_assert_int_eq(result[i].bits[j], tmp.bits[j]);
    ck_assert_int_eq(ret, 0);
  }
}
END_TEST

Suite *Create_suite_s21_negate() {
  Suite *suite = suite_create("s21_decimal_negate tests");
  TCase *tcase_core = tcase_create("Core");
  tcase_add_test(tcase_core, test);
  suite_add_tcase(suite, tcase_core);
  return suite;
}