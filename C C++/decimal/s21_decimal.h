#include <stddef.h>


// макрос чтоб создавать варажения вида:
//      QUAD(A)     -> 0xAAAAAAAA
//      QUAD(A, B)  -> 0xABBBBBBB
// используется кое-где в .c
#define QUAD2(f, s, ...) 0x##f##s##s##s##s##s##s##s
#define QUAD(...) QUAD2(__VA_ARGS__, __VA_ARGS__)

typedef struct {
  unsigned bits[4];
} s21_decimal;


int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);


// эти функции нужны были для того чтобы в ручну тестить их
// поидеи надо выпилить перед сдачей
#ifdef HAND_TEST
int cmt(s21_decimal* a);
int cdt(unsigned* bits, int round);
int nazelo(s21_decimal* a, s21_decimal b, unsigned* c);
int poww(s21_decimal a);
void spw(s21_decimal *a, int powe);
int zero(s21_decimal a);
int neg(s21_decimal a);
int sign(s21_decimal a);
unsigned pcis(unsigned *a, unsigned b);
int cmt(s21_decimal *a);
int adif(s21_decimal *a, s21_decimal *b, int rei);
int sdvig(unsigned *a, int *platz, const int neg);
#endif
