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


#define START(T) {                                      \
    printf("TIME  TEST %10s:  ", #T); fflush(stdout);    \
    auto start = chrono::high_resolution_clock::now();

#define END                                             \
    auto stop  = chrono::high_resolution_clock::now();  \
    cout << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << endl; }

void accumulate(double* not_optimize) {
    double ans = 0;
    for(int i = 0; i < 100; i++) ans += not_optimize[i]*(i+1);
    cout << "Count test: " << ans << endl;
}

int main() {
    int Quantity = 100000, seeds[100];
    for(int i = 0; i < 100; i++) seeds[i] = rand();
    double not_optimize[100] = {0};
    
    START(tree)
    for(int chance = 5; chance < 91; chance++) 
        indel_test_trust<tree<char>>(Quantity, chance, not_optimize + chance, seeds[chance]);
    END
    accumulate(not_optimize);

    START(set)
    for(int chance = 5; chance < 91; chance++) 
        indel_test_trust<set<char>>(Quantity, chance, not_optimize + chance, seeds[chance]);
    END
    accumulate(not_optimize);

    // START(BinaryTree)
    // for(int chance = 5; chance < 91; chance++) indel_test_trust<s21::BinaryTree<char, char>>(10000, chance, not_optimize + chance);
    // END
    // accumulate(not_optimize);

    cout << "\nIS CORRECT WORK?:" << endl;
    for(int chance = 5; chance < 91; chance++) {
        printf("    del chance = %3d: ", chance); fflush(stdout);
        indel_test_verify<tree<char>>(100000, chance, not_optimize);
    }
    cout << "\nMeow!" << endl;
    return 0;
}
