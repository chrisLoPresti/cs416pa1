#ifndef sort_h
#define sort_h

#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

node *sort(node *, char *, int);
node *sortProcs(node *, int, char *);
void convertLinkListToArrayStructure();
void mergeSort(node** array,int low,int high);

#endif