
CC=x86_64-linux-gnu-gcc
obj-m += blink_leds.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CC=$(CC)

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean