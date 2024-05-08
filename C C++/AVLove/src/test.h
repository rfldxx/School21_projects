// if def CHEKING when function compare set's


// пока что кринж через CURR_TYPE и FUNC_NAME
// 1. CURR_TYPE tree<char, my_cmp<char>>
// 2. CURR_TYPE  set<char>


#define  FIRST(a, ...) a
#define NFIRST(a, ...) ,##__VA_ARGS__
#define RET(...) { printf(FIRST(__VA_ARGS__) "\n" NFIRST(__VA_ARGS__)); fflush(stdout); return; }
void FUNC_NAME(int count_of_test, int chance_to_del, int seed = 42) {
    srand(seed);
    CURR_TYPE test;

    vector<char> values;
    int n = 80;
    for(int i = 0; i < n; i++) {
        values.push_back('#'+i);
        test.insert('#'+i);
    }

    int sum_n = 0, count = 0;

    for(int i = 0; i < count_of_test; ) {
        bool something_happens;
        bool mode = (rand()%101 < chance_to_del);
        char x = values[rand()%n];
        if( rand()%101 < chance_to_del ) something_happens = test.erase (x);
                                  else   something_happens = test.insert(x).second;
        
        i += something_happens;

        int curr_n = 0;
        for(auto p = test.begin(); p != test.end(); ++p) curr_n++;

        if(!something_happens) continue;
        // if( i % (count_of_test/3) == 0) { printf("."); fflush(stdout); }
        sum_n += curr_n;
        count++;
    }

    // printf("  <n> = %4lf\n", sum_n * 1. / count); fflush(stdout);
}

#undef  FIRST
#undef NFIRST
#undef    RET
