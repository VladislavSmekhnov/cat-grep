CC=gcc
CFLAGS=-c -std=c11 -Wall -Werror -Wextra -g

s21_grep: main.o
	$(CC) main.o -o s21_grep -lpcre
	rm -f *.o

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
	rm -rf s21_grep
	rm -rf *.log