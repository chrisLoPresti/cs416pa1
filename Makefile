CC = gcc
CFLAGS = -Wall -Werror -fsanitize=address
THREADSLIB= -pthread
MATHLIB = -lm

all: mapred 

driver: driver.c
	$(CC) $(CFLAGS) -o mm driver.c

sort: sort.c
	$(CC) $(CFLAGS) -o mm sort.c

mapred: driver.c sort.c mapred.c 
	$(CC) $(CFLAGS) -c driver.c
	$(CC) $(CFLAGS) -c sort.c
	$(CC) $(CFLAGS) -o mapred mapred.c driver.o sort.o $(THREADSLIB) $(MATHLIB)

clean:
	rm -f driver.o sort.o mapred.o
	rm -f mapred