CC = gcc
CFLAGS = -Wall -Werror -fsanitize=address
THREADSLIB= -pthread
MATHLIB = -lm

all: mapred

threads: threads.c
	$(CC) $(CFLAGS) -o mm threads.c $(THREADSLIB)

nonMapReduce: non_mapreduce.c
	$(CC) $(CFLAGS) -o mm non_mapreduce.c $(THREADSLIB)

sort: sort.c
	$(CC) $(CFLAGS) -o mm sort.c

mapred: threads.c non_mapreduce.c sort.c mapred.c
	$(CC) $(CFLAGS) -c non_mapreduce.c $(THREADSLIB)
	$(CC) $(CFLAGS) -c threads.c $(THREADSLIB)
	$(CC) $(CFLAGS) -c sort.c
	$(CC) $(CFLAGS) -o mapred mapred.c threads.o sort.o non_mapreduce.o $(THREADSLIB) $(MATHLIB)

clean:
	rm -f threads.o sort.o mapred.o
	rm -f mapred 
	rm -r mapred.dSYM