
all: cp_files

cp_files: main.o copy_module.o
	gcc main.o copy_module.o -o cp_files

main.o: main.c
	gcc -c main.c

copy_module.o: copy_module.c
	gcc -c copy_module.c

clean:
	rm -rf *.o cp_files *.copy