CC = g++
CFLAGS = -Wall -g

all: lru fifo

lru: lru.o
	$(CC) $(CFLAGS) -o lru lru.o

fifo: fifo.o
	$(CC) $(CFLAGS) -o fifo fifo.o

lru.o: lru.cpp
	$(CC) $(CFLAGS) -c lru.cpp

fifo.o: fifo.cpp
	$(CC) $(CFLAGS) -c fifo.cpp

clean:
	rm -f *.o lru fifo

.PHONY: clean all