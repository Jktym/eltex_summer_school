
CC=x86_64-linux-gnu-gcc
obj-m += chardev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CC=$(CC)
	(sudo insmod chardev.ko)

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	(sudo rmmod chardev.ko)