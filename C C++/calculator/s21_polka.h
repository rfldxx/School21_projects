#ifndef S21_POLKA_H
#define S21_POLKA_H

#define maxLixem 1024

struct info_t {
    char* text;
    int mode;
    // <0 - код Ошибки
    //  0 - функция
    //  1 - скобки
    //  2 - +, -, *, /, ^   (arg1 f arg2)
    //  3 - число int
    //  4 - число double
    //  5 - что-то (x, там другое что-то)

    int pos;
};
typedef struct info_t info;

int     omaru_polka(const char* input, char* data_dst, info* result, int* Nresult, int* last_pls, char* log_info);

int     exist_x(const info* result, int Nresult);

void    write_polka(const info* result, int Nresult, char* log_info);

double* calculate_graph(const info* result, int Nresult, int last_pls, const int n, const double* x_val);

#endif  // S21_POLKA_H
