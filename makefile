make: scanner.o tree.o dictionary.o list.o
	gcc main.c scanner.o tree.o dictionary.o list.o -Wall -Wextra -std=c99 -g -o spice; dos2unix data.txt queries.txt;

scanner.o: scanner.c scanner.h
	gcc -c scanner.c -g 

tree.o: tree.c tree.h
	gcc -c tree.c -g 

dictionary.o: dictionary.c dictionary.h
	gcc -c dictionary.c -g 

list.o: list.c list.h
	gcc -c list.c -g 

test:
	make; ./spice

clean: 
	rm *.o spice
