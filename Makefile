CC = gcc
CFLAGS = -Wall -Werror -fsanitize=address
THREADSLIB= -pthread
MATHLIB = -lm

all: mapred

threads: threads.c
	$(CC) $(CFLAGS) -o mm threads.c $(THREADSLIB)

sort: sort.c
	$(CC) $(CFLAGS) -o mm sort.c

mapred: threads.c sort.c mapred.c
	$(CC) $(CFLAGS) -c threads.c $(THREADSLIB)
	$(CC) $(CFLAGS) -c sort.c
	$(CC) $(CFLAGS) -o mapred mapred.c threads.o sort.o $(THREADSLIB) $(MATHLIB)

clean:
	rm -f threads.o sort.o mapred.o mapred 
	rm -r mapred.dSYM