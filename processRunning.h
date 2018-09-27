#ifndef processRunning_h
#define processRunning_h

#include "structs.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

void initializeProcessMemory(node **, int, int, int, char *);
void forkChildren();
#endif