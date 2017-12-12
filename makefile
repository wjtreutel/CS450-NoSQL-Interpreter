make: scanner.o dictionary.o list.o
	gcc main.c scanner.o dictionary.o list.o -Wall -Wextra -std=c99 -g -o spice; dos2unix data.txt queries.txt;

scanner.o: scanner.c scanner.h
	gcc -c scanner.c -g 

dictionary.o: dictionary.c dictionary.h
	gcc -c dictionary.c -g 

list.o: list.c list.h
	gcc -c list.c -g 

test:
	make; ./spice

clean: 
	rm *.o spice

slim: 
	make; rm *.o
