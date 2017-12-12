OBJS = scanner.o dictionary.o list.o
OOPTS = -Wall -Wextra -g -c 
LOPTS = -Wall -Wextra -g

all: helloWorld

wjtreutel : $(OBJS)
	gcc $(LOPTS) -o wjtreutel $(OBJS)


scanner.o: scanner.c
	gcc $(OOPTS) scanner.c

dictionary.o: dictionary.c
	gcc $(OOPTS) dictionary.c

list.o: list.c
	gcc $(OOPTS) list.c

clean:
	rm -r $(OBJS) test-[rpkect

test:
	wjtreutel
