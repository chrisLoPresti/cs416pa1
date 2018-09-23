CC = gcc
CFLAGS = -Wall -Werror -fsanitize=address

all: mapred

threads: threads.c
	$(CC) $(CFLAGS) -o mm threads.c

sort: sort.c
	$(CC) $(CFLAGS) -o mm sort.c

mapred: threads.c sort.c mapred.c
	$(CC) $(CFLAGS) -c threads.c
	$(CC) $(CFLAGS) -c sort.c
	$(CC) $(CFLAGS) -o mapred mapred.c threads.o sort.o
	rm -f threads.o sort.o 
	rm -r mapred.dSYM
clean:
	rm -f threads.o sort.o mapred