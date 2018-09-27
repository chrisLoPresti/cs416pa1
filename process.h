#ifndef process_h
#define process_h

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

void initializeProcessMemory(node **, int, int, int, char *, int);
void forkChildren();
char *itoa(int);
#endif