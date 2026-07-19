
all: calc

calc: main.o calc.o
	gcc main.o calc.o -o calc

main.o: main.c
	gcc -c main.c

calc.o: calc.c
	gcc -c calc.c

clean:
	rm -rf *.o' calc

