
#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
/*
 * This struct will hold all of our input
 */
struct inputData
{
    char * word;
    struct inputData *next;
    
}typedef inputData;

inputData * insertInput(inputData *, char*);
void parseInputFile();