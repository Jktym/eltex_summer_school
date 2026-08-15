
all: phonebook

phonebook: main.o libphonebook.so
	gcc main.o -L. -lphonebook -Wl,-rpath,. -o phonebook

libphonebook.so: lib/menu.o lib/phonebook.o
	gcc -shared lib/menu.o lib/phonebook.o -o libphonebook.so

main.o: main.c lib/menu.h
	gcc -c main.c -I./lib -o main.o

lib/menu.o: lib/menu.c lib/menu.h
	gcc -c -fPIC lib/menu.c -o lib/menu.o

lib/phonebook.o: lib/phonebook.c lib/phonebook.h
	gcc -c -fPIC lib/phonebook.c -o lib/phonebook.o

clean: 
	rm -f *.o lib/*.o *.so phonebook