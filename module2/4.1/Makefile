all: phonebook

phonebook: main.o menu.o phonebook.o 
	gcc -Wall main.o menu.o phonebook.o -o phonebook

main.o: main.c
	gcc -c main.c

menu.o: menu.c 
	gcc -c menu.c

phonebook.o: phonebook.c
	gcc -c phonebook.c

test: phonebook_test.o unity.o test_phonebook.o
	gcc phonebook_test.o unity.o test_phonebook.o -o test_result
	./test_result

phonebook_test.o: phonebook.c phonebook.h
	gcc -DCONTACTS_FILE=\"test_contacts.txt\" -c phonebook.c -o phonebook_test.o

unity.o: unity.c
	gcc -c unity.c -o unity.o

test_phonebook.o: test_phonebook.c
	gcc -c test_phonebook.c -o test_phonebook.o

rebuild: clean all

clean:
	rm -rf *.o phonebook test_result