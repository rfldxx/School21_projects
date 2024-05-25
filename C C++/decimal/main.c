#include <stdio.h>
#include <stdlib.h>
#include "s21_decimal.h"

#define info(...) do {              \
printf("\n\n\n------------------[");\
printf(__VA_ARGS__);                \
printf("]------------------\n");    \
} while(0)


//ui
int** elka;
int  size(void* gde);
void* ibm(int n, int one_size);
void free2(void** a);
void init(int m);
void show_int_no_enter(int x);
void show_int(int x);
void show_bits(s21_decimal a);
void show(s21_decimal a);
void copy_show(unsigned* a, int n, int* result);
s21_decimal create(char* number);


void test_slack(){
    s21_decimal a = create("+001212.49992881");
    s21_decimal r;
    show(a);

    printf("\nfloor  -->\n");
    s21_floor(a, &r);
    show(r);

    printf("\nround  -->\n");
    s21_round(a, &r);
    show(r);

    printf("\ntruncate  -->\n");
    s21_truncate(a, &r);
    show(r);
}


void test_sdvig(const s21_decimal a);
void test_adif (char* aa,  char* bb);

#define un_pack(...) __VA_ARGS__
#define CREATE(aa, bb) s21_decimal a = {{un_pack aa}}, b = {{un_pack bb}};

int main() {
    init(250);
    
    
    s21_decimal r;
    int rr;
    
    s21_decimal a = create("+12391.4");
    s21_decimal b = create("+0.03");
    
    cmt(&a);
    cdt(a.bits, 0);
    
    show(a);
    show(b);
    info("result :   %d", s21_div(a, b, &r));
    show(r);
    
    //test_sdvig(a);
    return 0;
    
    printf("END!");
    free2((void**) elka);
    elka = NULL;
    return 0;
}






//--------------------------------------------------------------------------------
int size(void* gde) {return *(((int*)gde) - 1);}

void* ibm(int n, int one_size) {
    int* extra = malloc(sizeof(int) + n * one_size);
    *extra++ = n;
    return extra;
}

void free2(void** a){
    for(int i = 0; i < size(a); i++) free( ((int*)a[i]) - 1 );
    free( ((int*)a) - 1 );
}

void init(int m){
    elka = ibm(m, sizeof(int*));
    int xxx[100] = {1}, n = 1;

    for(int pow = 0, prev; pow < m; pow++){
        if(n >= 100) printf("NOOOOO!\n");

        prev = 0;
        for(int i = 0; i < n && pow != 0; i++){
            xxx[i] *= 2;
            xxx[i] += prev;
            prev = xxx[i]/10;
            xxx[i] %= 10;
        }
        if(prev) xxx[n++] = prev;

        elka[pow] = ibm(n, sizeof(int));
        for(int t = 0; t < n; t++) elka[pow][t] = xxx[t];
    }
}

void show_int_no_enter(int x){
    for(int i = 31; i >= 0; i--){
        if( ! ((i+1) % 8) ) printf(" ");
        printf("%d", ((1 << i) & x) != 0);
    }
}

void show_bits(s21_decimal a) {
    printf("decimal:\n");
    for(int iii = 0; iii < 4; iii++){
        show_int_no_enter(a.bits[iii]);
        printf("  (0x%08X)\n", a.bits[iii]);
    }
    printf("\n\n");
}

void show(s21_decimal a){
    int result[250] = {0}, pos = 50;
    copy_show(a.bits, 3, result);
    
    for(; pos && !result[pos] && pos != poww(a); pos--);
    printf("decimal: %c%*s", neg(a) ? '-' : '+', 50 - pos - (poww(a) > 0), "");

    for(; pos >= 0; pos--) {
        printf("%d", result[pos]);
        if(pos && pos == poww(a)) printf(".");
    }
    while(pos-- >= poww(a)) printf("0");

    printf("  (pow %d) (sign %d)\n", poww(a), sign(a));
}

void copy_show(unsigned* a, const int n, int* result) {
    unsigned no_given[250] = {0}, showw = 0;
    if(!result) {result = no_given; showw = 1;}

    int pow2 = 0;
    for(int p = 0; p < n; p++) { // ˜ÂÊ ‚ ÒË ÌÂÚ ·ËÚÓ‚˚ı ÚËÔÓ‚
        unsigned curent = a[p];

        for(unsigned one = 1; one; one <<= 1, pow2++)  // Ò‰‚Ë„Ë
            for(int irl = 0; curent & one && irl < size(elka[pow2]); irl++)
                result[irl] += elka[pow2][irl];
    }

    int prev = 0, pos = 0;
    for(pos = 0; pos < size(elka[pow2]); pos++){
        result[pos] += prev;
        prev = result[pos] / 10;
        result[pos] %= 10;
    }
    while(prev) {
        result[pos++] = prev % 10;
        prev /= 10;
    }
    
    if(showw) {
        pos--;
        for(; pos && result[pos] == 0; pos--) printf(" ");
        for(; pos >= 0; pos--) printf("%d", result[pos]);
        printf("\n");
    }
}

s21_decimal create(char* number) {
    s21_decimal result = {{0}};
    
    if(*number == '+' || *number == '-') result.bits[3] = (*number++ == '-') << 31;

    char* dot = NULL;
    for(; *number && (*number == '.' || cmt(&result)); number++){
        if(*number == '.') dot = number;
        else pcis(result.bits, *number - '0');
    }

    if(dot) spw(&result, number - dot - 1);
    return result;
}

void test_sdvig(const s21_decimal a) {
    printf("IN START\n");
    show(a);
    
    int shift = 0, useless = 0;
    s21_decimal max_a;
    for(int i = 0; i < 4; i++) max_a.bits[i] = a.bits[i];
    
//    for(int i = 0; i< 40; i++) {
//    printf("\nx2^%d %d\n", i, sdvig(max_a.bits+2, &shift, 0));
//    show(max_a);
//    }
//
//    return;
    
    
    while(sdvig(max_a.bits+2, &shift, 0));
    
    printf("max_a\n* 2^%d\t", shift); show(max_a); show_bits(max_a);
    printf("\n");
    
    while(sdvig(max_a.bits, &shift, 1)) {
        printf("--------------------------------------------------------------------------------\n/ 2^%d\t", shift); show(max_a); show_bits(max_a);
        
        s21_decimal new_a;
        for(int i = 0; i < 4; i++) new_a.bits[i] = a.bits[i];
        for(int i = 0; i < shift; i++) sdvig(new_a.bits+2, &useless, 0);
        printf("* 2^%d\t", shift); show(new_a); show_bits(new_a);
        printf("\n");
    }
    
    printf("orignl:\t"); show(a); show_bits(a);
}

void test_adif(char* aa, char* bb) {
    s21_decimal a = create(aa);
    s21_decimal b = create(bb);
    printf("a is\t"); show(a); show_bits(a);
    printf("b is\t"); show(b); show_bits(b);
    
    int t = adif(&a, &b, 0);
    printf("a %c b !!!\n", t == 0 ? '=' : (t > 0 ? '>' : '<'));
}

