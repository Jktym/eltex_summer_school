

obj-m += proc.o
CC=x86_64-linux-gnu-gcc

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CC=$(CC)

rebuild: clean all

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean