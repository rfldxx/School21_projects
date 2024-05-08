#include "AVLtree.h"

template<typename T>
struct my_cmp {
    bool operator() (T a, T b) { return a < b; }
};

// СДЕЛАТь map_cmp:
template<typename T>
struct map_cmp {
    bool operator() (T a, T b) { return a.first < b.first; }
};


template<typename TreeIterator>
pair<int, bool> calc_h(TreeIterator iter) {
    if(!iter.currr()) return {0, 0};
    int lh = calc_h(iter.nexxt(0)).first;
    int rh = calc_h(iter.nexxt(1)).first;
    if(lh > rh) swap(lh, rh);
    return {rh + 1, rh - lh <= 1};
}


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
            
//            printf(">>> Erase  %c\n", x);
//            vlr(test.zero.next[0]);
            
        } else {
            auto [pp, tt] = test.insert(x);
            if(*pp != x) { printf("Fail position in insert\n"); return; }
            something_happens = tt;

            ideal.insert(x);
            
//            printf(">>> Insert %c\n", x);
//            vlr(test.zero.next[0]);
        }

        // [test] in [ideal]
        for(auto p = test.begin(); p != test.end(); ++p) {
            char finding = *p;
            if(!ideal.count(finding)) { printf("Waste elements in tree\n"); return; }
        }

        // [ideal] in [test]
        for(auto elem : ideal) {
            auto prev = test.find(elem);
            if( *prev != elem ) { printf("Tree didn't contain all the elements\n"); return; }
        }

        i += something_happens;

        int max_h = 0, curr_n = 0;
        for(auto p = test.begin(); p != test.end(); ++p) {
            auto [curr_h, AVLcheck] = calc_h(p);
            if(p.hight() != curr_h) { printf("FATTALL ERROR with hight!!!!!\n"); return; }
            if( !AVLcheck ) { printf("NOT A AVL TREE!!!!!\n"); return; }

            max_h = max(p.hight(), max_h);
            curr_n++;
        }
        if(max_h+1 != test.end().hight() ) { printf("ERROR WITH ROOT!!!!!  %d != %d\n",  test.end().hight(), max_h+1); return; }


        if(!something_happens) continue;
        if( i % (count_of_test/10) == 0) { printf("."); fflush(stdout); }
        sum_h += max_h;
        sum_n += curr_n;
        count++;
    }
    

    printf("  <h> = %4lf  <n> = %4lf", sum_h * 1. / count, sum_n * 1. / count); fflush(stdout);
}

int main() {

#ifdef TEST
    for(int chance = 5; chance < 91; chance++) {
        printf("\nchance = %3d: ", chance); fflush(stdout);
        test_updt_h(100000, chance);
    }

    cout << "\nMeow!" << endl;
    return 0;
#endif

}
