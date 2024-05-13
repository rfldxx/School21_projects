#include "AVLtree.h"
#include "over_avl.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <set>

#include <chrono>


template<typename T>
struct my_cmp {
    bool operator() (T a, T b) { return a < b; }
};

// СДЕЛАТь map_cmp:
template<typename T>
struct map_cmp {
    bool operator() (T a, T b) { return a.first < b.first; }
};


template <class T_set_1, class T_set_2>
bool check_sets(T_set_1& a, T_set_2& b) {
    // [a] in [b]
    for(auto p = a.begin(); p != a.end(); ++p) 
        if( *(b.find(*p)) != *p ) return 0;
    
    // [b] in [a]
    for(auto p = b.begin(); p != b.end(); ++p) 
        if( *(a.find(*p)) != *p ) return 0;

    return 1;
}

template<typename TreeIterator>
pair<int, bool> calc_h(TreeIterator iter) {
    if(!iter.currr()) return {0, 0};
    int lh = calc_h(iter.nexxt(0)).first;
    int rh = calc_h(iter.nexxt(1)).first;
    if(lh > rh) swap(lh, rh);
    return {rh + 1, rh - lh <= 1};
}


#define  CAT(...) wCAT(__VA_ARGS__)
#define wCAT(a, b) a##b
#define NAME trust
#include "test.h"
#undef  NAME
#define NAME verify  
#define CHECKING
#include "test.h"


// ==========================================
const int Repeats [] = {  50000,    50000}; 
const int Quantity[] = {1000000, 10000000};
// ==========================================

int seeds[100];
double not_optimize[100] = {0};

double accumulate(double* not_optimize) {
    double ans = 0;
    for(int i = 0; i < 100; i++) ans += not_optimize[i]*(i+1);
    return ans;
}


#define START { auto start = chrono::high_resolution_clock::now();
#define END                                             \
    auto stop  = chrono::high_resolution_clock::now();  \
    cout << chrono::duration_cast<chrono::milliseconds>(stop - start).count(); }

void chance_interval(int epoch, int a, int b = -1) {
    if(b == -1) b = a+10;

    printf("     [%3d, %3d, ", a, b);
    START
    for(int chance = a; chance < b; chance++) 
        indel_test_trust<set<double>>(Repeats[epoch], Quantity[epoch], chance, not_optimize + chance, seeds[chance]);
    END
    double check_accumulate1 = accumulate(not_optimize);

    printf(", ");
    START
    for(int chance = a; chance < b; chance++) 
        indel_test_trust<tree<double>>(Repeats[epoch], Quantity[epoch], chance, not_optimize + chance, seeds[chance]);
    END
    double check_accumulate2 = accumulate(not_optimize);
    printf("]\n");

    if(check_accumulate1 != check_accumulate2) printf("ERROR");
}


int main() {
    for(int epoch = 0; epoch < sizeof(Repeats) / sizeof(int); epoch++) {
        for(int i = 0; i < 100; i++) seeds[i] = rand();

        printf("== repeat: %7d    quantity: %7d  =========\n", Repeats[epoch], Quantity[epoch]);
        printf("%*s stl set | my AVL\n", 11, "");
        chance_interval(epoch,  5, 10);
        chance_interval(epoch, 10, 15);
        chance_interval(epoch, 15, 25);
        chance_interval(epoch, 25, 35);
        chance_interval(epoch, 35, 45);
    }

    cout << "\nMeow!" << endl;
    return 0;
}
