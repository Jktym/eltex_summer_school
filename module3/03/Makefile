
all: p2p_chat

p2p_chat: main.o p2p_chat.o
	gcc main.o p2p_chat.o -o p2p_chat -lrt

main.o: main.c
	gcc -c main.c

p2p_chat.o: p2p_chat.c
	gcc -c p2p_chat.c

clean: rm -rm *.o p2p_chat