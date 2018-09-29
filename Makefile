CC = gcc
CFLAGS = -Wall -Werror -fsanitize=address
THREADSLIB= -pthread
MATHLIB = -lm

all: mapred 

process: process.c
	$(CC) $(CFLAGS) -o process.c

threads: threads.c
	$(CC) $(CFLAGS) -o mm threads.c $(THREADSLIB)

sort: sort.c
	$(CC) $(CFLAGS) -o mm sort.c

mapred: process.c threads.c sort.c mapred.c
	$(CC) $(CFLAGS) -c process.c
	$(CC) $(CFLAGS) -c threads.c $(THREADSLIB)
	$(CC) $(CFLAGS) -c sort.c
	$(CC) $(CFLAGS) -o mapred mapred.c process.o threads.o sort.o $(THREADSLIB) $(MATHLIB)

clean:
	rm -f threads.o sort.o mapred.o
	rm -f mapred