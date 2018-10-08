#ifndef mapred_h
#define mapred_h

#include "structs.h"
#include "sort.h"
#include <math.h>
#include <signal.h>
#include <pthread.h>
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
#include <sys/wait.h>
#include <ctype.h>

//mapper is whats called from our main file to map our input
void mapper(node *, int, int, int, char *, int, char *);
//called from mapper to set up posix mem
void createSharedMemory(int);
//functions called in mapper to create key pairs associated by index
void generateKeysProcs();
void generateKeysThreads();
void *generateKeyPair(void *);
//function to manage the reducing
void reducer();
void generateReducersThreads();
void generateReducersProcs();
void *reduceCombined(void *);
void finalReducer();
//write output to file
void processWriteToFile();
void processIndividualWrite(char *, int);
//extra
void generateExtraThreads();
void *createTree(void *);
treeNode *insertIntoTree(treeNode *, char *);
void printAndFree(treeNode *);

#endif