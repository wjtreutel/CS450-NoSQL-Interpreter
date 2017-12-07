make: scanner.o support.o dictionary.o list.o
	gcc main.c scanner.o support.o dictionary.o list.o -Wall -Wextra -std=c99 -g -o spice

scanner.o: scanner.c scanner.h
	gcc -c scanner.c -g 

support.o: support.c support.h
	gcc -c support.c -g 

dictionary.o: dictionary.c dictionary.h
	gcc -c dictionary.c -g 

list.o: list.c list.h
	gcc -c list.c -g 

test:
	make; ./spice

clean: 
	rm *.o spice
