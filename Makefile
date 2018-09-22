CC = gcc
CFLAGS = -g -Wall

all: mapred

mymalloc: threads.c
	$(CC) $(CFLAGS) -o mm threads.c

mapred: threads.c mapred.c
	$(CC) $(CFLAGS) -c threads.c
	$(CC) $(CFLAGS) -o mapred mapred.c threads.o
clean:
	rm -f threads.o mapred