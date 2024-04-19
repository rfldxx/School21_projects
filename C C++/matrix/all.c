#include "s21_matrix.h"
#define Am A->matrix
#define Bm B->matrix
#define rm result->matrix
#define r ->rows
#define c ->columns

#define FORij(i, j, s, t,...) for(int i = s; i < A r; i++) __VA_ARGS__ for(int j = t; j < A c; j++)
#define FOR(...) FORij(i, j, 0, 0,  __VA_ARGS__)

int s21_create_matrix(int rows, int columns, matrix_t *A) {
    if(!A || (A r = rows) < 1 || (A c = columns) < 1) return 1;
   // if(Am) free(Am); // что за тупые условия, это надо или нет?!
    if( !( Am = (double**)malloc(rows*sizeof(double*) + rows*columns*sizeof(double)) )) return 2;
    FOR({Am[i] = (double*) (Am + rows) + i*columns;) Am[i][j] = 0;}
    return 0;
}
        void s21_remove_matrix(matrix_t *A) { if((A) && (Am)) {free(Am); Am = NULL;} A r = A c = 0;}

#define second(a, b, ...) b
#define wsecond(...) second(__VA_ARGS__)
#define FIND(a) ,a
int bred(matrix_t* A) {return !A || A r < 1 || A c < 1 || !Am; }
#define CHECK(er1, er2, ...) if(bred(A) || bred(B)) return er1; if(wsecond( \
    FIND __VA_ARGS__ (A r != B r || A c != B c), A c != B r) ) return er2;

#define EVOLVE(name, SOMETHING, MAIN, EXIT, ...)    \
int s21_##name(matrix_t *A ,##__VA_ARGS__) {        \
    SOMETHING; FOR() MAIN; return EXIT;            \
}

EVOLVE(eq_matrix, CHECK(FAILURE, FAILURE), if( fabs(Am[i][j] - Bm[i][j]) > EPS) return FAILURE, SUCCESS, matrix_t* B);

#define MATRIX_SLACK(name, extra, op, ind1, ind2)                               \
EVOLVE(name##_matrix, CHECK(1, 2, extra); s21_create_matrix(A r, B c, result),  \
       extra rm[i][ind2] += Am[i][j] op Bm[ind1][ind2], 0, matrix_t* B, matrix_t *result)
MATRIX_SLACK(sum,  , +, i, j);
MATRIX_SLACK(sub,  , -, i, j);
MATRIX_SLACK(mult, for(int k = 0; k < B c; k++), *, j, k);;

#define shortcut(n, m) if(bred(A)) return 1; s21_create_matrix(A n, A m, result)
EVOLVE(mult_number, shortcut(r, c), rm[i][j] = Am[i][j]*number, 0, double number, matrix_t *result);
EVOLVE(transpose, shortcut(c, r), rm[j][i] = Am[i][j], 0, matrix_t* result);

#define maxN 64
#define principi_programirovania matrix_t* B = A; CHECK(1, 2, xxx);

int s21_calc_complements(matrix_t *A, matrix_t *result) {
    principi_programirovania;
    s21_create_matrix(A r, A c, result);
    
    FOR() {
        double slope[2*maxN] = {0}, *temp;
        slope[maxN] = 1; //(i+j) % 2 ? -1 : 1;
        temp = Am[i];
        Am[i] = slope+maxN - j;
        
        s21_determinant(A, rm[i]+j);
        Am[i] = temp;
    }
    
    return 0;
}

#define SIZE(N) (N)*sizeof(double*) + (N)*(N)*sizeof(double)
#define no_goto(...) {memcpy(Am, temp, SIZE(A c)); __VA_ARGS__;}
int s21_determinant(matrix_t *A, double *result) {
    principi_programirovania;
    char temp[SIZE(maxN)];
    memcpy(temp, Am, SIZE(A c));
    *result = 1;
        
    FORij(k, l, 0, k)
        if(fabs(Am[l][k]) > EPS) {
            *result *= Am[l][k] * (l != k ? -1 : 1);
            FORij(i, j, k, k+1, if(i != l) )
                Am[i][j] -=  Am[l][j] * Am[i][k] / Am[l][k];
                
            Am[l] = Am[k];
            break;
        } else if(l + 1 == A c) no_goto(*result = 0; return 0);
        
    no_goto(return 0);
}
     
int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
    principi_programirovania;
    double det;
    if(s21_determinant(A, &det) || fabs(det) < EPS) return 2;
    matrix_t temp1, temp2;
    s21_calc_complements(A, &temp1);
    s21_transpose(&temp1, &temp2);
    s21_mult_number(&temp2, 1. / det, result);
    s21_remove_matrix(&temp1);
    s21_remove_matrix(&temp2);
    return 0;
}

