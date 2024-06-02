COMP=gcc -Wall -Werror -Wextra ../common/both_stuff.h reset_grep.c -o s21_grep

all: 
	rm -rf s21_grep 
	gcc -DCUTEE=0 -Wall -Werror -Wextra ../common/both_stuff.h reset_grep.c -o s21_grep


.PHONY: s21_grep
s21_grep:
	rm -rf s21_grep 
	$(COMP) -DCUTEE=0

say_nice:
	rm -rf s21_grep
	$(COMP) -DCUTEE=1

do_test:
	rm -rf s21_grep
	$(COMP) -DCUTEE=0
	touch s21_grep.txt
	touch grep.txt
	sh test/s21_grep_tests.sh
	rm -rf s21_grep.txt	
	rm -rf grep.txt

clear: 
	rm -rf *.txt
	rm -rf s21_grep