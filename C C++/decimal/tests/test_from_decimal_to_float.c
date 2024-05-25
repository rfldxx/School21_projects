#include <check.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "s21_decimal.h"

static s21_decimal src[] = {
    {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
    {{0x0000000B, 0x00000000, 0x00000000, 0x80010000}},
    {{0x0000000B, 0x00000000, 0x00000000, 0x00010000}},
    {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}},
    {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}},
    {{0x000000D3, 0x00007C45, 0x00007C57, 0x80080000}},
    {{0x003A6E50, 0x007F1483, 0x003215CF, 0x00020000}},
    {{0x00000000, 0x00000000, 0x00000000, 0x800A0000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x00140000}},
};

static float dst[] = {
    0.f,
    -1.10000002f,
    1.10000002f,
    79228162500000000000000000000.f,
    -79228162500000000000000000000.f,
    -5871782930000000.f,
    605492757000000000000000.f,
    0.f,
    9.99999968E-21f,
};

START_TEST(from_decimal_to_float_test) {
  for (size_t i = 0; i < sizeof(src) / sizeof(s21_decimal); ++i) {
    
      
    float tmp;
    int ret = s21_from_decimal_to_float(src[i], &tmp);
    if (tmp != dst[i]) {
      char buf1[512];
      char buf2[512];
      sprintf(buf1, "%.6E", tmp);
      sprintf(buf2, "%.6E", dst[i]);
      ck_assert(strcmp(buf1, buf2) == 0);
    } else {
      ck_assert_float_eq(tmp, dst[i]);
      ck_assert_int_eq(ret, 0);
    }
  }
}
END_TEST

Suite *Create_suite_s21_from_decimal_to_float() {
  Suite *suite = suite_create("from_decimal_to_float tests");
  TCase *tcase_core = tcase_create("Core");
  tcase_add_test(tcase_core, from_decimal_to_float_test);
  suite_add_tcase(suite, tcase_core);
  return suite;
}
