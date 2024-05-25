#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define space(a, n) for(int ii = 0; ii < n; ii++) printf("%c", a);
#define enter printf("\n");

#define un_pack(...) __VA_ARGS__
#define CREATE(aa, bb) s21_decimal a = {{un_pack aa}}, b = {{un_pack bb}};

struct func {
    char name[100];
    int (*f) (s21_decimal*, s21_decimal, s21_decimal*, int*, float*);
};

int s21_cmt(s21_decimal* a) {return cmt(a);}
int s21_cdt(s21_decimal* a, int round) {return cdt(a->bits, round);}
int s21_nazelo(s21_decimal* a, s21_decimal b, s21_decimal* r) {
    b.bits[3] &= 0xFF00FFFF;  
    b.bits[3] |= 0x00FF0000 & (poww(*a) << 16);

    r->bits[0] = r->bits[1] = r->bits[2] = r->bits[3] = 0;
    return nazelo(a, b, r->bits);
}

#define blank(name, short, ...)                                                     \
int my_##short(s21_decimal* a, s21_decimal b, s21_decimal* r, int* d, float* x) {    \
/*printf("in function we have:\n");  show(*a); enter; show(b); enter;*/ \
    return s21_##name(__VA_ARGS__);                                                 \
}

blank(add, a, *a, b, r) blank(sub, s, *a, b, r)
blank(mul, m, *a, b, r) blank(div, d, *a, b, r)
blank(nazelo, nazelo, a, b, r)

#define blank2(name, short) blank(is_##name, short, *a, b)
blank2(less,    l) blank2(less_or_equal,    le)
blank2(greater, g) blank2(greater_or_equal, ge)
blank2(equal,   e) blank2(not_equal,        ne)

#define blank3(t1, t2, xx)                          \
blank(from_##t1##_to_##t2, t1##_to_##t2, *xx,  r)   \
blank(from_##t2##_to_##t1, t2##_to_##t1, *a,  xx)
blank3(int,   decimal, d)
blank3(float, decimal, x)

blank(floor,    floor,    *a, r) blank(round,  round,  *a, r)
blank(truncate, truncate, *a, r) blank(negate, negate, *a, r)

blank(cmt, cmt,       a)
blank(cdt, cdt,       a,  0)
blank(cdt, cdt_round, a, *d)

#define mutate2(a, b, ...) {a, my_##b}
#define mutate(a, ...) mutate2(#a ,##__VA_ARGS__, a ,##__VA_ARGS__)
struct func info[] = {
    mutate(+, a), mutate(-, s), mutate(*, m), mutate(/, d),
    mutate(nazelo),

    {"<" , my_l}, {"<=", my_le},
    {">" , my_g}, {">=", my_ge},
    {"==", my_e}, {"!=", my_ne},
    
    mutate(int_to_decimal), mutate(float_to_decimal),
    mutate(decimal_to_int), mutate(decimal_to_float),
    
    mutate(floor),    mutate(round),
    mutate(truncate), mutate(negate),
    
    mutate(cmt), mutate(cdt), mutate(cdt_round)
};
int size_info = sizeof(info) / sizeof(struct func);


int skip_comment_reading(FILE* f, char* write) {
    int res;
    while( (res = fscanf(f, "%s", write) != EOF) && write[0] == '/' && write[1] == '/'){
        while((fscanf(f, "%c", write) != EOF) && *write != '\n');
    }
    return res;
}

int read_buf(FILE* f, char* buf){
    if(!skip_comment_reading(f, buf)) return 0;
    
    int i = strlen(buf), drive = buf[0] == '{' && buf[i-1] != '}';
    if(drive) {
        while((fscanf(f, "%c", buf+i) != EOF) && (drive -= buf[i++] == '}'));
        buf[i] = 0;
    }
    
    return 1;
}

void run(char* name) {
    FILE* f = fopen(name, "r");
    
    char buf1[100], op[100], buf2[100], c;
    int line = 0;
    while(read_buf(f, buf1) && skip_comment_reading(f, op)) {
        line++;
        printf(" [%2d]  ", line);
        
        int i = 0, is_sec;
        
        int sec_int = !strcmp(op,        "cdt_round");
        if(is_sec = (strlen(op) <= 2 || !strcmp(op, "nazelo") || sec_int) ) read_buf(f, buf2);
            
        for(; strcmp(op, info[i].name) && i < size_info; i++);
        if(i == size_info) {
            printf("    problem with find operation (%s)!!!\n\n\n", op);
            while((fscanf(f, "%c", &c) != EOF) && c != '\n');
            continue;
        }
        
        int fir_int = !strcmp(op,   "int_to_decimal");
        int res_int = !strcmp(op,   "decimal_to_int");
        
        int fir_flt = !strcmp(op, "float_to_decimal");
        int res_flt = !strcmp(op, "decimal_to_float");
        
        s21_decimal a, b, r;
        if(!fir_int && !fir_flt) a = create(buf1);
        
        int   d = fir_int ? atoi(buf1) : 0;
        float x = fir_flt ? atof(buf1) : 0;
        
        if(is_sec){
            if(sec_int) d =   atoi(buf2);
            else        b = create(buf2);
        }
        
        
        // show
        int w = 90, fs = 7;
                        space('/', w); enter;
        space(' ', fs); space('/', 3 ); space(' ', 4); printf("operation      %s", op); enter;
        space(' ', fs); space('/', w); enter;
        
        space(' ', fs); space('/', 3 ); space(' ', 4); printf("init:"); enter;
        space(' ', fs); space('/', 3 ); space(' ', 7);
            if(fir_int) printf("%d", d);
            else if(fir_flt) printf("%.6E", x);
            else show(a);
        enter;
        
        if(is_sec) {
            space(' ', fs); space('/', 3 ); space(' ', 7);
           
            if(sec_int) {
                switch(d) {
                    case 0:
                        printf("no round"); break;
                    case 1:
                        printf("bankovskoe round"); break;
                    case 2:
                        printf("adekvatnoe round"); break;
                    default:
                        printf("TROUBLE! round (%d)", d);
                }
            } else show(b);
            
            enter;
        } else if(!strcmp(op, "cdt")) {
            space(' ', fs); space('/', 3 ); space(' ', 7);
            printf("no round");
            enter;
        }
        
        space(' ', fs); space('/', 3 ); enter;
        space(' ', fs); space('/', 3 ); space(' ', 4);
            printf("result:   %d", info[i].f(&a, b, &r, &d, &x));
        enter;
        
        int already = 1;
        space(' ', fs); space('/', 3 ); space(' ', 7);
            if(res_int)              {already = 0; printf("%d", d);}
            else if(res_flt)         {already = 0; printf("%.6E", x);}
            else if(i < 5 || 10 < i) {already = 0; show(i > 18 ? a : r);}
        enter;
        
        if(!strcmp(op, "nazelo")) {
            space(' ', fs); space('/', 3 ); enter;
            space(' ', fs); space('/', 3 ); space(' ', 4); printf("ostatok:"); enter;
            space(' ', fs); space('/', 3 ); space(' ', 7); show(a); enter;
        }
        
        
        if(!already) {
            space(' ', fs); space('/', 3 ); enter;
        }
        space(' ', fs); space('/', w); enter;

        enter; enter;
    }
    
    fclose(f);
}

int main() {
    init(250);
    
//    s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
//    s21_decimal b = {{5, 0, 0, 0x00010000}};
//    s21_decimal r;
//
//    show(a); enter;
//    show(b); enter;
//    s21_sub(a, b, &r);
//
//    show(r);

    run("hand_test.txt");
    
    printf("END!\n");
    free2((void**) elka);
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

    printf("  (pow %d) (sign %d)", poww(a), sign(a));
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
    
    if(*number == '{'){
        number++;
        //printf(">%s<\n", number);
        char c;
        for(int i = 0; i < 4; i++) {sscanf(number, "%i  %c", result.bits+i, &c);
            //printf("scan 0x%08X %c\n", result.bits[i], c);
            
            while(*number && *number++ != ',');
            //  printf(">%s<\n", number);
        }
        return result;
    }
        
    if(*number == '+' || *number == '-') result.bits[3] = (*number++ == '-') << 31;

    char* dot = NULL;
    for(; *number && (*number == '.' || cmt(&result)); number++){
        if(*number == '.') dot = number;
        else pcis(result.bits, *number - '0');
    }

    if(dot) spw(&result, number - dot - 1);
    
    return result;
}

