
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
/*
 * This struct will hold all of our input
 */
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

bucket *insertInput(bucket *, char *);
void parseInputFile();
void initializeBuckets();