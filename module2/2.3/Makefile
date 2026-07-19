
all: calc

calc: main.o calc.o menu.o
	gcc main.o calc.o menu.o -o calc

main.o: main.c
	gcc -c main.c

menu.o: menu.c
	gcc -c menu.c

calc.o: calc.c
	gcc -c calc.c

unity_test: test.o unity.c calc.o menu.o
	gcc -c test.c unity.c calc.c menu.c
	gcc test.o unity.o calc.o menu.o -o test_result -lm

clean:
	rm -rf *.o calc test_result

