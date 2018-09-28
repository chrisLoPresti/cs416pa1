#ifndef process_h
#define process_h

#include "structs.h"
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

void createSharedMemory(node **, int, int, int);
void mapProcs(int);
void syncLists();

#endif