make: scanner.o main.c
	gcc main.c scanner.o -Wall -Wextra -std=c99 -g -o spice

scanner.o: scanner.c scanner.h
	gcc -c scanner.c
