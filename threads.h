#ifndef threads_h
#define threads_h

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

struct inputData
{
    char *word;
    struct inputData *next;

} typedef inputData;

struct bucket
{
    int id;
    struct inputData *keys;
    struct bucket *next;
} typedef bucket;

void initializeMemory(bucket *, int, int);

#endif