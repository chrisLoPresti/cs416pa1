#ifndef process_h
#define process_h

#include "structs.h"
#include "sort.h"
#include <math.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <semaphore.h>

void procsDriver(node **, int, int, int, char *, int);
void createSharedMemory();
void mapProcs(int);
void reduceProcs(int, int);
void finalReducer();
void processWriteToFile();
void processIndividualWrite(char *, int);
void createMaps();
void createReduces();

#endif