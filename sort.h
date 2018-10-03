#ifndef sort_h
#define sort_h

#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//threads
void myMergeSortDriverThreads(node **, char *);
void splitMapResults(node *, node **, node **);
node *myMergeSortThreads(node *, node *, char *);
//procs
node *myMergeSortDriverProcs(node *, node *, int, char *);
void myMergeSortProcs(char *, node *, node *, int, int);
void myMergeProcs(char *, node *, node *, int, int, int);

#endif