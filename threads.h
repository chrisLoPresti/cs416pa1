#ifndef threads_h
#define threads_h

#include "sort.h"
#include "structs.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

void initializeThreadMemory(bucket *, int, int, int);
void produceThreads();
void *map(void *);

#endif