
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

struct node
{
    char * word;
    int count;
    struct node *next;
    
}typedef node;

inputData * insertInput(inputData *, char*);
void parseInputFile();

// functions used for sorting
node *sort(node *);
void testSort();
