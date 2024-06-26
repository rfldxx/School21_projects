// if def CHECKING when function compare set's


#ifdef CHECKING
#define RET(cond, ...) if(cond) { printf(__VA_ARGS__); printf("\n"); fflush(stdout); return; }
#define PRNT(...) printf(__VA_ARGS__); fflush(stdout);
#else
#define RET(...)
#define PRNT(...)
#endif


template <typename T>
void CAT(indel_test_, NAME) (int count_of_test, int count_of_value, int chance_to_del, double* not_optimize, int seed = 42) {
    srand(seed);
    T test;

#ifdef CHECKING
    set<typename T::value_type> ideal;
#endif

    vector<typename T::value_type> values;
    int n = count_of_value;
    for(int i = 0; i < n; i++) 
        values.push_back( rand() * 1. / (abs(rand()) + 1) ); //  <- по идеи бы сюда тоже шаблоную функцию, но лень


    // printf("after filling:  "); fflush(stdout);
    // auto start = chrono::high_resolution_clock::now();

    int sum_h = 0, sum_n = 0, count = 0;
    for(int i = 0; i < count_of_test; ) {
        bool something_happens;
        bool mode = (rand()%101 < chance_to_del);
        auto x = values[rand()%n];
        if( rand()%101 < chance_to_del ) {
            something_happens = test.erase (x);
#ifdef CHECKING
            ideal.erase(x);
#endif
        } else {
            auto [pp, tt] = test.insert(x);
            RET(*pp != x, "Fail position in insert");
            something_happens = tt;
#ifdef CHECKING
            ideal.insert(x);
#endif
        }

        RET(!check_sets(test, ideal), "NOT EQUAL SETS!!!!!");
        i += something_happens;

        int curr_n = 0, max_h = 0;
        for(auto p = test.begin(); p != test.end(); ++p) {
#ifdef CHECKING
            auto [curr_h, AVLcheck] = calc_h(p);
            RET(p.hight() != curr_h, "FATTALL ERROR with hight!!!!!");
            RET(!AVLcheck, "NOT A AVL TREE!!!!!");
            max_h = max(p.hight(), max_h);
#endif
            curr_n++;
        }

        RET(max_h+1 != test.end().hight(), "ERROR WITH hight of ROOT!!!!!  %d != %d", test.end().hight(), max_h+1);

        if(!something_happens) { continue; }
        if( i % (count_of_test/10) == 0) { PRNT("."); }
        sum_h += max_h;
        sum_n += curr_n;
        count++;

        // if( !i % (count_of_test/10) ) { printf("."); fflush(stdout); }
    }

    // auto stop  = chrono::high_resolution_clock::now();  
    // cout << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << endl;
    
#ifdef CHECKING
    printf("  <h> = %4lf  <n> = %4lf\n", sum_h * 1. / count, sum_n * 1. / count); fflush(stdout);
#endif
    *not_optimize = sum_n * 1. / count;
}


#undef RET
#undef PRNT
