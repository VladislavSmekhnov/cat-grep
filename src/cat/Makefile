CC=gcc
CFLAGS=-c -std=c11 -Wall -Werror -Wextra -g

s21_cat: main.o
	$(CC) main.o -o s21_cat
	rm -f *.o

test:
	./test_cat/test_func_cat.sh > cat_func_tests.log
	./test_cat/test_leak_cat.sh > cat_leak_tests.log
	tail -n 3 cat_func_tests.log cat_leak_tests.log
	echo ""
	rm *.log

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
	rm -rf s21_cat
	rm -rf *.log
