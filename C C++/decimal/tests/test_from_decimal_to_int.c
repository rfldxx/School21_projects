#include <check.h>
#include <stdio.h>

#include "s21_decimal.h"




void test_dec(s21_decimal value, int res, int ret) {
  int dst = 0;
  int ret1 = s21_from_decimal_to_int(value, &dst);
    
//    if(dst != res) printf( "val    %d != %d\n", dst, res);
//    if(ret1 != ret) printf("return %d != %d\n", ret1, ret);
    
  if(!ret) ck_assert_int_eq(dst, res);
  ck_assert_int_eq(ret1, ret);
}

static s21_decimal src1[] = {
    {{0x0000007B, 0x00000000, 0x00000000, 0x00000000}},
    {{0xFFFFFFFF, 0x000003E7, 0x00000000, 0x00030000}},
    {{0x00000000, 0x00000001, 0x00000000, 0x00000000}},
    {{0x00000000, 0x00000001, 0x00000000, 0x80000000}},
    {{0x000003E7, 0x00000000, 0x00000000, 0x80030000}},
    {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},
    {{0x000003E7, 0x000001F4, 0x00000000, 0x80030000}},
    {{0x000F4240, 0x00002710, 0x00002710, 0x00000000}},
    {{0x000F4240, 0x00002710, 0x00002710, 0x80000000}},
    {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},
    {{0x00000619, 0x00000000, 0x00000000, 0x00010000}},
    {{0x00000619, 0x00000000, 0x00000000, 0x80010000}},
};

static int dst1[] = {123, -1, 0, 0, 0, -1, -2147483648, 0, 0, 0, 156, -156};
static int err1[] = {
    0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0,
};
START_TEST(s21_from_decimal_to_int_test) {
  for (size_t i = 0; i < sizeof(src1) / sizeof(s21_decimal); i++) {
     // printf("%d\n", i);
    test_dec(src1[i], dst1[i], err1[i]);
  }
}
END_TEST

Suite *Create_suite_s21_from_decimal_to_int() {
  Suite *suite = suite_create("from_decimal_to_int tests");
  TCase *tcase_core = tcase_create("Core");
  tcase_add_test(tcase_core, s21_from_decimal_to_int_test);
  suite_add_tcase(suite, tcase_core);
  return suite;
}
