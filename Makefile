CFLAGS = -Wall -std=c11 -D_XOPEN_SOURCE=500 -O0 -g
LFLAGS = -lrt -pthread

.PHONY: clean

all: producer consumer

producer: prod.o
	$(CC) $^ $(LFLAGS) -o $@

consumer: cons.o
	$(CC) $^ $(LFLAGS) -o $@

prod.o: main.c
	$(CC) -c $< $(CFLAGS) -DPRODUCER -o $@

cons.o: main.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f *.o producer consumer
