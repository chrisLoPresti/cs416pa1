#ifndef driver_h
#define driver_h

#include "structs.h"
#include "mapred.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
//reads the input file and hands our keys off to the mapper
node *insertInput(node *, char *);
void parseInputFile();
int validinput(char);
void generateArray();

#endif