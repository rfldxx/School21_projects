#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct {
    char title[30];
    int  xp;
} project[] = { {"CPP1_name1", 300},  {"CPP2_name2",  750},  {"SQL0_name3", 800},  {"AAA0_name4", 800},
                {"ZZZ0_name5", 800},  {"SQL1_name6",  300},  {"AAX1_name7", 500},  {"AAX2_name8", 300},
                {"AAA1_name9", 500},  {"CPP3_extra", 1500} };

#define default_in  "info.txt"
#define default_out "psql.txt"

FILE*  in;
FILE* out;

int  ii, no_comment;
char line[500];
char name[100][50];
int crash_line() {
    if( !fgets(line, 500, in)) return -1;

    ii = 0;
    no_comment = 1;
    for(int add = 0; sscanf(line + add, "%s", name[ii]) == 1 && (no_comment = strcmp(name[ii], "//")); ii++) {
        while(line[add] == ' ') add++;
        add += strlen(name[ii]);
    }

    return ii;
}

// если max опыт кратен 10, то ничего не должно сломаться от округления
int generate_xp(int what_prj) {
    return project[what_prj-1].xp * (name[ii-1][0] == '+' ? 80 + rand()%20 : rand()%80) / 100;
}


#define   max_id(xxx) "(SELECT COALESCE(max(id),0) FROM " #xxx ")"
#define print_id(coma_flag, xxx, format, ...)  do { \
    if( !coma_flag) fprintf(out, ",");              \
    else coma_flag = 0;                             \
    fprintf(out, "\n    ( " max_id(xxx) " + %2d, " format ")", rltv_id++, __VA_ARGS__); \
} while(0)


void friends() {
    int rltv_id = 1, first_add = 1; 
    fprintf(out, "INSERT INTO friends VALUES");

    while( crash_line() > 0 ) {
        printf("  group: {%s", name[0]);
        for(int i = 1; i < ii; i++) printf(", %s", name[i]);
        printf("}\n");

        for(int i = 0; i < ii-1; i++)
            for(int j = i+1; j < ii; j++)
                print_id(first_add, friends, "'%s', '%s'", name[i], name[j]);
    }

    fprintf(out, ";\n\n\n");
}

void recommendations() {
    int rltv_id = 1, first_add = 1; 
    fprintf(out, "INSERT INTO recommendations VALUES");

    while( crash_line() > 0 ) {
        printf("  %s: {%s", name[0], name[1]);
        for(int i = 2; i < ii; i++) printf(", %s", name[i]);
        printf("}\n");

        for(int i = 1; i < ii; i++) {
            int vzm = (name[i][0] == '=') + 2*(name[i][0] == '<');
            if( vzm < 2 ) print_id(first_add, recommendations, "'%s', '%s'", name[0]      , name[i]+!!vzm);
            if( vzm )     print_id(first_add, recommendations, "'%s', '%s'", name[i]+!!vzm, name[0]      );
        }
    }

    fprintf(out, ";\n\n\n");
}


#define time_skip "x"
void timetracking () {
    int rltv_id = 1, first_add = 1; 
    fprintf(out, "INSERT INTO timetracking VALUES");

    while( crash_line() > 0 ) {    
        printf("  date: '%s'\n", name[0]);

        if (!first_add) fprintf(out, ",\n");
        else first_add = 0;
        fprintf(out, "\n--");
        for(int i = 1; i < ii; i++) fprintf(out, " %s", name[i]);

        int chetnost[100] = {0};

        int time = 0, extra_first_flag = 1;
        for(int i = 1; i < ii; i++) {
            int counter = 1, mlt = ('0' <= name[i][0] && name[i][0] <= '9');
            if( mlt ) {
                sscanf(name[i], "%d", &counter);
                counter--;
            }
            
            while(counter--) {
                if( !strcmp(name[i-mlt], time_skip) ) { time++; continue; }

                for(int j = i-1; 1 <= j; j--)
                    if( !strcmp(name[j], name[i-mlt]) ) {
                        chetnost[i] = (chetnost[j]+1)%2;
                        break;
                    }
                if(mlt) chetnost[i-1] = chetnost[i];

                print_id(extra_first_flag, timetracking, "'%s', '%s', '%02d:10', %d", name[i-mlt], name[0], time++, 1+chetnost[i]);
            }
        }

        // закрываем оставшиеся сессии
        for(int i = 1; i < ii; i++) 
            if( (name[i][0] < '0' || '9' < name[i][0]) && strcmp(time_skip, name[i]) ) {
                printf("    analyze: %s(%2d)", name[i], i);

                for(int j = ii-1; i <= j; j--)
                    if( !strcmp(name[j], name[i]) ) {
                        printf("  =>  find last: %s(%2d)[cond = %d]", name[j], j, 1+chetnost[j]);

                        if(!chetnost[j]) {
                            printf("  =>  close session");
                            chetnost[j] = 1;
                            print_id(extra_first_flag, timetracking, "'%s', '%s', '%02d:10', 2", name[i], name[0], time++);
                        }
                        break;
                    }
                printf("\n");
            }
    }

    fprintf(out, ";\n\n\n");
}

#define start "  'start'"
#define suces "'success'"
#define faill "'failure'"

char date[500] = "2022-01-21";

void checks () {
    int rltv_id = 1, first_add = 1; // minutes = 5; 

    while( crash_line() > 0 ) {
        fprintf(out, "-- %s", line);
        int wat_prj;
        sscanf(name[1], "%d", &wat_prj);
        fprintf(out, "INSERT INTO checks VALUES ( " max_id(checks) " + 1, '%s', '%s', '%s');\n", name[0], project[wat_prj-1].title, date);

        int time = 0;
        int result = 2 - 2*(name[ii-1][0] == '#') - (name[ii-1][0] == '~'); // 0 - fail, 1 - work, 2 - suces

        // [ P2P ] ================================================================================
        fprintf(out, "INSERT INTO    p2p VALUES");
        int crivoy_flag = 1;
        #define SAME_LINE(xxx, ind) print_id(crivoy_flag, p2p, max_id(checks) ", '%s', " xxx ", '%02d:10'", name[ind], time++)

        SAME_LINE(start, 2);

        int l = ii - (name[ii-2][0] == 'V');
        for(int i = 3; i < l-1; i++) {
            SAME_LINE(start, i);
            SAME_LINE(suces, i-1);
        }

        if(l != ii || result == 2)  SAME_LINE(suces, l-2);
        else if(!result)            SAME_LINE(faill, l-2);
        fprintf(out, ";\n");

        // [ VERTER ] =============================================================================
        if(l != ii) {
            fprintf(out, "INSERT INTO verter VALUES ( " max_id(verter) " + 1, " max_id(checks) ", " start ", '%02d:10')",      time++);
            if(result == 2)  fprintf(out, ",\n %24c ( " max_id(verter) " + 2, " max_id(checks) ", " suces ", '%02d:10')", ' ', time++);
            if(result == 0)  fprintf(out, ",\n %24c ( " max_id(verter) " + 2, " max_id(checks) ", " faill ", '%02d:10')", ' ', time++);
            fprintf(out, ";\n");
        }

        // [ XP ] =================================================================================
        if(result == 2) {
            fprintf(out, "INSERT INTO     xp VALUES ( " max_id(xp) " + 1, " max_id(checks) ", %d);\n", generate_xp(wat_prj));
        }

        fprintf(out, "\n\n");
    }
}

int main(int argc, char* argv[]) {
    printf("Count of args: %d\n", argc);
     in = fopen( argc >= 2 ? argv[1] : default_in , "r");
    out = fopen( argc >= 3 ? argv[2] : default_out, "w");

    while( crash_line() >= 0) {
        if(ii == 0 && no_comment) continue;

        printf("\n\nNEW ITERATION: >%s<\n", name[0]);

            if ( !strcmp(name[0],         "friends:") )  friends();
       else if ( !strcmp(name[0], "recommendations:") )  recommendations(); 
       else if ( !strcmp(name[0],    "timetracking:") )  timetracking();  
       else if ( !strcmp(name[0],              "//" ) )  printf("  REACH comnt: %s", line+3);
       else if ( !strcmp(name[0],          "checks:") ) {
            strcpy(date, name[1]);           
            printf("  date:  >%s<\n", date);
            checks();
        } 
       else { printf("ERROR: %s\n", name[0]); break; }
    }
    
    fclose( in);
    fclose(out);
}
 
