#include <check.h>
#include <stdio.h>

#include "check.h"

void check_decimal(s21_decimal a, s21_decimal b) {
    ck_assert_int_eq(s21_is_equal(a, b), 1);
    
    int prew = 1;
    if(poww(a) > poww(b)) {
        int n = poww(a) - poww(b);
        while(n) {
            prew &= cmt(&b);
            n--;
        }
        spw(&b, poww(a));
    } else if(poww(b) > poww(a)) {
        int n = poww(b) - poww(a);
        while(n) {
            prew &= cmt(&a);
            n--;
        }
        spw(&a, poww(b));
    }
    ck_assert_int_eq(prew, 1);

    
  int no_zero_a = 0, no_zero_b = 0;
  for (int i = 0; i < 3; i++) {
    no_zero_a |= a.bits[i];
    no_zero_b |= b.bits[i];
  }

  ck_assert_int_eq(no_zero_a, no_zero_b);

  if (no_zero_b) ck_assert_int_eq(a.bits[3], b.bits[3]);
}

