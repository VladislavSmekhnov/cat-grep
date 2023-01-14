CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -c
OS = $(shell uname -s)
TEST_DIR = grep_tests
FUNC_TESTS = test_func_grep
LEAKS_TESTS = test_leak_grep

ifeq ($(OS), Darwin)
	SH_CMD = sh
endif

ifeq ($(OS), Linux)
	SH_CMD = bash
endif

s21_grep: main.o
	$(CC) main.o -o s21_grep -lpcre
	rm main.o

test: s21_grep
	$(SH_CMD) $(TEST_DIR)/$(FUNC_TESTS).sh > $(FUNC_TESTS).log 2>&1
	$(SH_CMD) $(TEST_DIR)/$(LEAKS_TESTS).sh > $(LEAKS_TESTS).log 2>&1
	tail -n 3 $(FUNC_TESTS).log $(LEAKS_TESTS).log
	rm $(FUNC_TESTS).log $(LEAKS_TESTS).log

main.o: main.c
	$(CC) $(CFLAGS) main.c

clang:
	cp ../../materials/linters/.clang-format ../
	clang-format -n *.c *.h

clang-replace:
	cp ../../materials/linters/.clang-format ../
	clang-format -i *.c *.h

clean:
	rm -rf main.o
	rm -rf s21_grep
	rm -rf *.log
	clear
