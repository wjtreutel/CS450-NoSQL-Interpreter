make: scanner.o support.o
	gcc main.c scanner.o support.o -Wall -Wextra -std=c99 -g -o spice

scanner.o: scanner.c scanner.h
	gcc -c scanner.c -g 

support.o: support.c support.h
	gcc -c support.c -g 

test:
	make; ./spice

clean: 
	rm *.o spice
