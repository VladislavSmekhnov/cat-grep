CC=gcc
CFLAGS=-c -std=c11 -Wall -Werror -Wextra -g

s21_grep: main.o
	$(CC) main.o -o s21_grep -lpcre
	rm -f *.o

test:
	sh grep_tests/test_func_grep.sh > grep_func_tests.log 2>&1
	sh grep_tests/test_leak_grep.sh > grep_leak_tests.log 2>&1
	tail -n 3 grep_leak_tests.log grep_leak_tests.log
	rm *.log

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
	rm -rf s21_grep
	rm -rf *.log