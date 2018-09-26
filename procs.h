#ifndef procs_h
#define procs_h

#include "sort.h"
#include "structs.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>

void *map(void *);
void writeToFile();

#endif