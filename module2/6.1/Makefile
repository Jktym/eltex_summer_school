
all: phonebook

phonebook: main.o libphonebook.a
	gcc main.o -L. -lphonebook -o phonebook

libphonebook.a: lib/menu.o lib/phonebook.o
	ar rc libphonebook.a lib/menu.o lib/phonebook.o

main.o: main.c lib/menu.h
	gcc -c main.c -I./lib -o main.o

lib/menu.o: lib/menu.c lib/menu.h
	gcc -c lib/menu.c -o lib/menu.o

lib/phonebook.o: lib/phonebook.c lib/phonebook.h
	gcc -c lib/phonebook.c -o lib/phonebook.o

clean: 
	rm -f *.o lib/*.o *.a phonebook