#ifndef sort_h
#define sort_h

#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

node *sort(node *, char *);
node *myMergeSort(node *, node *, int, char *);
void myMergeSortDriver(char *, node *, node *, int, int);
void myMerge(char *, node *, node *, int, int, int);

#endif