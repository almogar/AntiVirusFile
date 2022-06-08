all: menu

menu: task2b.o
	gcc -m32 -g -Wall -o menu task2b.o

task2b.o: task2b.c
	gcc -m32 -g -Wall -c -o task2b.o task2b.c

.PHONY: clean

clean:
	rm -f *.o menu
