#include "AVLtree.h"

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



// Супер не поканонам, чисто для быстренького теста
#define CURR_TYPE tree<char, my_cmp<char>>
#define FUNC_NAME tree_test
#include "test.h"
#undef  CURR_TYPE
#undef  FUNC_NAME
#define CURR_TYPE set<char>
#define FUNC_NAME set_test
#include "test.h"

#define  FIRST(a, ...) a
#define NFIRST(a, ...) ,##__VA_ARGS__
#define RET(...) { printf(FIRST(__VA_ARGS__) "\n" NFIRST(__VA_ARGS__)); fflush(stdout); return; }
void test_updt_h(int count_of_test, int chance_to_del) {
    tree<char, my_cmp<char>> test;
    set<char> ideal;

    vector<char> values;
    int n = 80;
    for(int i = 0; i < n; i++) {
        values.push_back('#'+i);
        test.insert('#'+i);
        ideal.insert('#'+i);
    }

    int sum_h = 0, sum_n = 0, count = 0;

    for(int i = 0; i < count_of_test; ) {
        bool something_happens;
        bool mode = (rand()%101 < chance_to_del);
        char x = values[rand()%n];
        if( rand()%101 < chance_to_del ) {
            something_happens = test.erase (x);
            ideal.erase(x);
            
            
        } else {
            auto [pp, tt] = test.insert(x);
            if(*pp != x) RET("Fail position in insert");
            something_happens = tt;

            ideal.insert(x);
            
        }

        if( !check_sets(test, ideal) ) RET("NOT EQUAL SETS!!!!!");

        i += something_happens;

        int max_h = 0, curr_n = 0;
        for(auto p = test.begin(); p != test.end(); ++p) {
            auto [curr_h, AVLcheck] = calc_h(p);
            if(p.hight() != curr_h) RET("FATTALL ERROR with hight!!!!!");
            if( !AVLcheck ) RET("NOT A AVL TREE!!!!!");

            max_h = max(p.hight(), max_h);
            curr_n++;
        }
        if(max_h+1 != test.end().hight() ) RET("ERROR WITH hight of ROOT!!!!!  %d != %d\n",  test.end().hight(), max_h+1);


        if(!something_happens) continue;
        if( i % (count_of_test/10) == 0) { printf("."); fflush(stdout); }
        sum_h += max_h;
        sum_n += curr_n;
        count++;
    }
    

    printf("  <h> = %4lf  <n> = %4lf", sum_h * 1. / count, sum_n * 1. / count); fflush(stdout);
}



int main() {
    {
        printf("TIME TEST TREE: "); 
        auto start = chrono::high_resolution_clock::now();
        for(int chance = 5; chance < 91; chance++)  tree_test(100000, chance);
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        cout << duration.count() << endl;
    }
    {
        printf("TIME TEST SET : "); 
        auto start = chrono::high_resolution_clock::now();
        for(int chance = 5; chance < 91; chance++)  set_test(100000, chance);
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        cout << duration.count() << endl;
    }



#ifdef TEST
    for(int chance = 5; chance < 91; chance++) {
        printf("\nchance = %3d: ", chance); fflush(stdout);
        test_updt_h(100000, chance);
    }

    cout << "\nMeow!" << endl;
    return 0;
#endif

}
