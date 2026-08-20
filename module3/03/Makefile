
all: p2p_chat

p2p_chat: main.o 
	gcc main.o -o p2p_chat -lrt

main.o: main.c
	gcc -c main.c

rebuild: clean all

clean: 
	rm -f *.o p2p_chat