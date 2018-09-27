#ifndef procs_h
#define procs_h

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
#include <math.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

void *map(void *);
void writeToFile();
char* initializeSHM();
char* getSHMAddr(int shm_id);
int startProcessing(node **newBuckets, int newMapsOrThreads, int newReduces, int output, char *application);

#endif
