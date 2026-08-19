
all: queue

queue: main.o menu.o queue.o package_generator.o

main.o: main.c
	gcc -c main.c -o main.o

queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o

package_generator.o: package_generator.c package_generator.h
	gcc -c package_generator.c -o package_generator.o

rebuild: clean all

clean:
	rm -f *.o queue