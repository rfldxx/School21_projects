#include <check.h>
#include <stdio.h>

#include "check.h"

#define un_pack(...) __VA_ARGS__
#define NIGHTCORE(i, err, aa, bb, rr)                                       \
  START_TEST(test_##i) {                                                    \
    s21_decimal a = {{un_pack aa}}, b = {{un_pack bb}}, r = {{un_pack rr}}; \
    s21_decimal tmp;                                                        \
    int ret = s21_div(a, b, &tmp);                                          \
    if (!ret) check_decimal(tmp, r);                                        \
    ck_assert_int_eq(ret, err);                                             \
  }                                                                         \
  END_TEST

NIGHTCORE(1, 3, (0x00000000, 0x00000000, 0x00000000, 0x00000000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000))

NIGHTCORE(2, 3, (0x00000000, 0x00000000, 0x00000000, 0x800A0000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000))

NIGHTCORE(3, 3, (0x00000000, 0x00000000, 0x00000000, 0x801C0000),
          (0x00000000, 0x00000000, 0x00000000, 0x800D0000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000))

NIGHTCORE(4, 3, (0x48127127, 0x91283271, 0xAB8130FC, 0x80000000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000),
          (0x48127127, 0x91283271, 0xAB8130FC, 0x80000000))

NIGHTCORE(5, 0, (0x00000000, 0x00000000, 0x00000000, 0x800D0000),
          (0x48127127, 0x91283271, 0xAB8130FC, 0x80000000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000))

NIGHTCORE(6, 0, (0x00000000, 0x00000000, 0x00000000, 0x001C0000),
          (0x48127127, 0x91283271, 0xAB8130FC, 0x800A0000),
          (0x00000000, 0x00000000, 0x00000000, 0x00000000))

NIGHTCORE(7, 2, (0x48127127, 0x91283271, 0xAB8130FC, 0x80000000),
          (0xFD203120, 0x00012912, 0x00012938, 0x001C0000),
          (0x48127127, 0x91283271, 0xAB8130FC, 0x80000000))

NIGHTCORE(8, 0, (0x48127127, 0x91283271, 0xAB8130FC, 0x801A0000),
          (0xFD203120, 0x00012912, 0x00012938, 0x001C0000),
          (0x0FD9FB4B, 0x4CA1CD58, 0x7A30F41A, 0x80160000))

NIGHTCORE(9, 0, (0x48127127, 0x91283271, 0xAB8130FC, 0x80000000),
          (0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000),
          (0xA8078C59, 0xC8932062, 0x15A59D3E, 0x801C0000))

NIGHTCORE(10, 0, (0x48127127, 0x91283271, 0xAB8130FC, 0x801C0000),
          (0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000),
          (0x00000001, 0x00000000, 0x00000000, 0x801C0000))

NIGHTCORE(11, 0, (0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFAA0, 0x80000000),
          (0x48127127, 0x91283271, 0xAB8130FC, 0x80000000),
          (0xC7A8E05D, 0xCEF8F826, 0x303B147E, 0x001C0000))

NIGHTCORE(12, 0, (0x48127127, 0x00083271, 0x00000000, 0x00000000),
          (0xFD203120, 0x00012912, 0xFFFFFAA0, 0x001C0000),
          (0x119EC25B, 0x02AA4B47, 0x5E18F241, 0x000E0000))

#define ADD_TEST(i) tcase_add_test(tcase_core, test_##i)

Suite *Create_suite_s21_div() {
  Suite *suite = suite_create("s21_decimal_div tests");
  TCase *tcase_core = tcase_create("Core");
  ADD_TEST(1);
  ADD_TEST(2);
  ADD_TEST(3);
  ADD_TEST(4);
  ADD_TEST(5);
  ADD_TEST(6);
  ADD_TEST(7);
  ADD_TEST(8);
  ADD_TEST(9);
  ADD_TEST(10);
  ADD_TEST(11);
  ADD_TEST(12);
  suite_add_tcase(suite, tcase_core);
  return suite;
}

