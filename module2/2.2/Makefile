
all: calc

calc: main.o calc.o menu.o
	gcc main.o calc.o menu.o -o calc

main.o: main.c
	gcc -c main.c

calc.o: calc.c
	gcc -c calc.c

menu.o: menu.c
	gcc -c menu.c

unity_test: calc.c unity.c menu.c test.c 
	gcc -c unity.c test.c calc.c menu.c
	gcc menu.o calc.o unity.o test.o -o test_result -lm

rebuild: clean all

clean:
	rm -rf *.o calc test_result

