#include "threads.h"

node **buckets; // buckets used in reduce are circular and point to last node
node *dataLinkList;
pthread_mutex_t dataLock;
int mapsOrThreads;
int reduces;
int outputFile;
int totalNodes;
char *app;
int *reduceCountArray; // this int array is used to store how many nodes are needed for each bucket in the reduce stage

void threading_driver(node **newBuckets, int newMapsOrThreads, int newReduces, int output, char *application, int keyCount)
{
    // initialize all information
    {
        buckets = newBuckets;
        mapsOrThreads = newMapsOrThreads;
        reduces = newReduces;
        outputFile = output;
        totalNodes = keyCount;
        app = malloc(strlen(application) + 1);
        strcpy(app, application);
    }
    // setup mutex lock
    initializeDataLinkListMutexLock();
    // create threads and run mapping function
    produceThreadMapsAndWaitTillAllThreadsFinish();
    // sort data
    dataLinkList = sort(dataLinkList, app);
    // printLinkList();
    generateHowManyNodesEachBucketWillContain();
    // printReduceCountArray();
    configureBucketsToContainCorrectNumberOfNodes();
    // printBuckets();
    produceReduceThreadsAndWaitTillAllThreadsFinish();
    // printBuckets();
    finalReduce();
    // printBuckets();
    writeToFile();
}

void initializeDataLinkListMutexLock()
{
    if (pthread_mutex_init(&dataLock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(EXIT_FAILURE);
    }
}

void produceThreadMapsAndWaitTillAllThreadsFinish()
{
    pthread_t mapperThread[mapsOrThreads];
    int i;
    for (i = 0; i < mapsOrThreads; i++)
    {
        if (buckets[i] && pthread_create(&mapperThread[i], NULL, map, (void *)buckets[i]) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < mapsOrThreads; i++)
    {
        pthread_join(mapperThread[i], NULL);
    }
}

void *map(void *keys)
{
    node *bucketData = (node *)keys;
    node *startptr = bucketData;
    node *lastptr;
    while (bucketData != NULL)
    {
        bucketData->count = 1;
        lastptr = bucketData;
        bucketData = bucketData->next;
    }

    pthread_mutex_lock(&dataLock);

    if (!dataLinkList)
    {
        dataLinkList = startptr;
    }
    else
    {
        lastptr->next = dataLinkList;
        dataLinkList = startptr;
    }
    pthread_mutex_unlock(&dataLock);

    return NULL;
}

void printLinkList()
{
    printf("Starting printing link list\n");
    node *tmp = dataLinkList;
    int x = 0;
    while (tmp != NULL)
    {
        printf("%s, %d\n", tmp->word, tmp->count);
        tmp = tmp->next;
        x++;
    }
    printf("total num of words: %d\n", x);
    printf("completed printing link list\n");
}

void generateHowManyNodesEachBucketWillContain()
{
    // printf("generating each bucket number\n");
    // create array which will store how many nodes each reduce thread will handle
    reduceCountArray = calloc(reduces + 1, sizeof(int));
    reduceCountArray[reduces] = '\0';

    int x;
    int reduceCountArrayIndex = 0;

    for (x = 0; x < totalNodes; x++)
    {
        if (reduceCountArrayIndex == reduces)
        {
            reduceCountArrayIndex = 0;
        }
        reduceCountArray[reduceCountArrayIndex] = reduceCountArray[reduceCountArrayIndex] + 1;
        reduceCountArrayIndex++;
    }
    // printf("finished generating each bucket number\n");
}

void printReduceCountArray()
{
    printf("Starting printing array of num\n");
    int x = 0;
    while (x < reduces)
    {
        printf("%d\n", reduceCountArray[x]);
        x++;
    }
    printf("completed printing array of num\n");
}

void configureBucketsToContainCorrectNumberOfNodes()
{
    // printf("starting configuring buckets\n");
    int x;
    for (x = 0; x < reduces; x++)
    {
        if (reduceCountArray[x] == '\0')
        {
            break;
        }
        node *head = dataLinkList;
        node *tmp = dataLinkList;
        int y;

        for (y = 0; y < reduceCountArray[x] - 1; y++) // minus one because error was caused without it
        {
            tmp = tmp->next;
        }
        // move main pointer to next starting position
        dataLinkList = tmp->next;

        // create circular link list in bucket
        buckets[x] = head;
        tmp->next = buckets[x];
        buckets[x] = tmp;
    }
    // printf("finished configuring buckets\n");
}

void printBuckets()
{
    printf("Starting printing buckets\n");
    int x;
    for (x = 0; x < reduces; x++)
    {
        printf("Printing data in bucket %d:\n", x);
        if (buckets[x] == NULL)
        {
            break;
        }
        node *tmp = buckets[x]->next;
        while (tmp != buckets[x])
        {
            printf("%s, %d\n", tmp->word, tmp->count);
            tmp = tmp->next;
        }
        printf("%s, %d\n", tmp->word, tmp->count);
    }
    printf("Ending printing buckets\n");
}

void produceReduceThreadsAndWaitTillAllThreadsFinish()
{
    // printf("tarting reduce threading\n");
    pthread_t reduceThread[reduces];
    int i;
    for (i = 0; i < reduces; i++)
    {
        reduceCountArray[i] = i;
        //passing in a void pointer that points to the address of reduceCountArray[i]
        if (buckets[i] && pthread_create(&reduceThread[i], NULL, reduce, (void *)&reduceCountArray[i]) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < reduces; i++)
    {
        pthread_join(reduceThread[i], NULL);
    }
    // printf("finished reduce threading\n");
}

void *reduce(void *num)
{
    //dereference that address pointer to get the int at the address
    int *bucketNumber = (int *)num;
    node *tail = buckets[*bucketNumber];
    node *head = tail->next;
    tail->next = NULL; // changes input list from circular to linear
    node *results = NULL;
    node *resultsPtr = NULL;

    while (head != NULL)
    {
        if (resultsPtr == NULL)
        {
            results = head;
            resultsPtr = head;
            head = head->next;
        }
        else
        {
            if (strcmp(resultsPtr->word, head->word) == 0)
            {
                resultsPtr->count = resultsPtr->count + head->count;
                node *tmp = head->next;
                free(head->word);
                free(head);
                head = tmp;
            }
            else
            {
                resultsPtr->next = head;
                resultsPtr = resultsPtr->next;
                head = head->next;
            }
        }
    }

    resultsPtr->next = results;
    buckets[*bucketNumber] = resultsPtr;
    return NULL;
}

void finalReduce()
{
    // printf("starting final reduce\n");
    int x;
    for (x = 0; x < reduces - 1; x++)
    {
        if (buckets[x + 1] != NULL && buckets[x] != NULL)
        {
            // if the last node in the bucket x has the same words as the first node in bucket x+1 then need to combine
            if (strcmp(buckets[x]->word, buckets[x + 1]->next->word) == 0)
            {
                buckets[x]->count = buckets[x]->count + buckets[x + 1]->next->count;
                // check if only one node in bucket
                if (buckets[x + 1]->next == buckets[x + 1])
                {
                    buckets[x + 1] = NULL;
                    moveBucketsToTheLeft(x + 1); // need to move all buckets over 1 to the left
                }
                else
                {
                    buckets[x + 1]->next = buckets[x + 1]->next->next;
                }
                // reset index to 0 but since for loop will add +1 at the end of the loop must set x -1
                x = -1;
            }
        }
    }
    // printf("finished final reduce\n");
}

void moveBucketsToTheLeft(int startingBucket)
{
    // printf("starting shifting buckets over\n");
    int x;
    for (x = startingBucket; x < reduces - 1; x++)
    {
        buckets[x] = buckets[x + 1];
    }
    buckets[reduces - 1] = NULL;
    // printf("finished shifting buckets over\n");
}

void writeToFile()
{
    int x;
    for (x = 0; x < reduces; x++)
    {
        if (buckets[x] == NULL)
        {
            break;
        }

        node *temp = buckets[x]->next;

        while (temp != buckets[x])
        {
            processWrite(temp);
            temp = temp->next;
        }
        processWrite(temp);
    }
}

void processWrite(node *temp)
{
    char buf[1000];
    if (write(outputFile, temp->word, strlen(temp->word)) < 0)
    {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }

    if (write(outputFile, " ", 1) < 0)
    {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buf, "%d", temp->count);
    if (write(outputFile, buf, strlen(buf)) < 0)
    {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }
    if (write(outputFile, "\n", 1) < 0)
    {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }
}