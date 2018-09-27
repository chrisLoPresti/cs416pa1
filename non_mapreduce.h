#ifndef non_mapreduce_h
#define non_mapreduce_h

#include "non_mapreduce.h"
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

void nonMapReduceDriver();
void nullAllBucketsPointers();
void deconstructBucketInToLinkList();
void generateHowManyNodesEachBucketWillContainForNonMapReduce();
void configureBucketsToContainCorrectNumberOfNodesForNonMapReduce();
void *nonMapReduce(void *);
void produceNonMapReduceThreadsAndWaitTillAllThreadsFinish();
void passBucketToWriteToFileForNonMapReduce();
void writeToFileForNonMapReduce(node *head);
void printBucketsForNonMapReduce();
void finalReduceForNonMapReduce();
void moveBucketsToTheLeftForNonMapReduce(int );
#endif