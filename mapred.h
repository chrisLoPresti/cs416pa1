#ifndef mapred_h
#define mapred_h

#include "structs.h"
#include "threads.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
/*
 * This struct will hold all of our input
 */

node *insertInput(node *, char *);
void parseInputFile();
void initializeBuckets();
int validinput(char);
void *cleanBuckets();

#endif