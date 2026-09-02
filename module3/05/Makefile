
all: producer consumer

producer: producer.c common.h
	gcc producer.c -o producer -lrt -pthread

consumer: consumer.c common.h
	gcc consumer.c -o consumer -lrt -pthread

clean:
	rm -f producer consumer