final: main.c scanner.o dictionary.o list.o
	gcc -Wall -Wextra -g main.c scanner.o dictionary.o list.o -o final

scanner.o: scanner.c
	gcc -Wall -Wextra -g -c scanner.c

dictionary.o: dictionary.c
	gcc -Wall -Wextra -g -c dictionary.c

list.o: list.c
	gcc -Wall -Wextra -g -c list.c

clean:
	rm *.o final

test:
	make; final
