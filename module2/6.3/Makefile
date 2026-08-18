
all: calc

calc: main.o menu_lib/libmenu.a dinamyc_libs/libadd.so dinamyc_libs/libsub.so dinamyc_libs/libmul.so dinamyc_libs/libdiv.so
	gcc main.o -L./menu_lib -lmenu -ldl -o calc

main.o: main.c
	gcc -c main.c -I./menu_lib -o main.o

menu_lib/libmenu.a: menu_lib/menu.o
	ar rc menu_lib/libmenu.a menu_lib/menu.o

menu_lib/menu.o: menu_lib/menu.c
	gcc -c menu_lib/menu.c -I./menu_lib -o menu_lib/menu.o

dinamyc_libs/libadd.so: dinamyc_libs/add.c
	gcc -shared -fPIC dinamyc_libs/add.c -o dinamyc_libs/libadd.so

dinamyc_libs/libsub.so: dinamyc_libs/sub.c
	gcc -shared -fPIC dinamyc_libs/sub.c -o dinamyc_libs/libsub.so

dinamyc_libs/libmul.so: dinamyc_libs/mul.c
	gcc -shared -fPIC dinamyc_libs/mul.c -o dinamyc_libs/libmul.so

dinamyc_libs/libdiv.so: dinamyc_libs/div.c
	gcc -shared -fPIC dinamyc_libs/div.c -o dinamyc_libs/libdiv.so

clean:
	rm -f *.o menu_lib/*.o menu_lib/*.a dinamyc_libs/*.so calc