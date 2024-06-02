COMP=gcc -DL  -Wall -Werror -Wextra ../common/both_stuff.h polished_cat.c -o s21_cat
all: s21_cat	

.PHONY: s21_cat
s21_cat: 
	rm -rf s21_cat
	$(COMP) -DCUTEE=0

say_nice:
	rm -rf s21_cat
	$(COMP) -DCUTEE=1 

do_test:
	rm -rf s21_cat
	$(COMP) -DCUTEE=0
	touch cat.txt
	touch s21_cat.txt
	sh test/s21_cat_tests.sh
	rm -rf cat.txt
	rm -rf s21_cat.txt

clear: 
	rm -rf *.txt
	rm -rf s21_cat