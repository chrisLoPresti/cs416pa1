#include "threads.h"

node **buckets; // buckets used in reduce are circular and point to last node 
node *dataLinkList;
node *dataLinkListHead;
pthread_mutex_t dataLock;
int mapsOrThreads;
int reduces;
int outputFile;
int totalNodes;
char *app;
int *reduceCountArray; // this int array is used to store how many nodes are needed for each bucket in the reduce stage

void threading_driver(node **newBuckets, int newMapsOrThreads, int newReduces, int output, char *application)
{
    // initialize all information 
    buckets = newBuckets;
    mapsOrThreads = newMapsOrThreads;
    reduces = newReduces; 
    outputFile = output;
    app = malloc(strlen(application) + 1);
    strcpy(app, application);

    // setup mutex lock
    initializeDataLinkListMutexLock();
 
    // create threads and run mapping function
    produceThreadMapsAndWaitTillAllThreadsFinish();

    // sort data
    dataLinkList = sort(dataLinkList, app);

    getTotalNodes();
    // printf("Total Nodes: %d\n", totalNodes);
    // printLinkList();
    // printf("reduces wanted: %d\n", reduces);
    generateHowManyNodesEachBucketWillContain();
    printReduceCountArray();
    configureBucketsToContainCorrectNumberOfNodes();
    // printBuckets();
    produceReduceThreadsAndWaitTillAllThreadsFinish();
    printBuckets();
    finalReduce();
    printBuckets();


    // writeToFile();
}


void getTotalNodes()
{
    node *tmp = dataLinkList;
    int x = 0;
    while (tmp != NULL)
    {
        x++;
        tmp = tmp->next;
    }
    totalNodes = x;
}

void printLinkList()
{
    printf("Starting printing link list\n");
    node *tmp = dataLinkList;
    int x = 0;
    while (tmp != NULL)
    {
        printf("%s, %d\n" , tmp->word, tmp->count);
        tmp = tmp->next;
        x++;
    }
    printf("total num of words: %d\n", x);
    printf("completed printing link list\n");
}

void printReduceCountArray()
{
    printf("Starting printing array of num\n");
    int x = 0;
    while (x < reduces)
    {
        printf("%d\n" , reduceCountArray[x]);
        x++;
    }
    printf("completed printing array of num\n");
}

void printBuckets()
{
    printf("Starting printing buckets\n");
    int x;
    for (x = 0; x < reduces; x++)
    {
        printf("Printing data in bucket %d:\n",x);
        if (buckets[x] == NULL)
        {
            break;
        }
        node *tmp = buckets[x]->next;
        while (tmp != buckets[x])
        {
            printf("%s, %d\n" , tmp->word, tmp->count);
            tmp = tmp->next;
        }
        printf("%s, %d\n" , tmp->word, tmp->count);
    }
    printf("Ending printing buckets\n");

}

void generateHowManyNodesEachBucketWillContain()
{
    printf("generating each bucket number\n");
    // create array which will store how many nodes each reduce thread will handle
    reduceCountArray = malloc( (sizeof(int) * reduces) + 1 * sizeof('\0'));
    reduceCountArray[reduces] = '\0';

    int x;
    int reduceCountArrayIndex = 0;
    // zero out all memory
    for(x = 0; x < reduces; x++)
    {
        reduceCountArray[x] = 0;
    }

    for(x = 0; x < totalNodes; x++)
    {
        if (reduceCountArrayIndex == reduces)
        {
            reduceCountArrayIndex = 0;
        }
        reduceCountArray[reduceCountArrayIndex] = reduceCountArray[reduceCountArrayIndex] + 1;
        reduceCountArrayIndex++;
    }
    printf("finished generating each bucket number\n");
}

void configureBucketsToContainCorrectNumberOfNodes()
{
    printf("starting configuring buckets\n");
    int x;
    for (x=0; x < reduces; x++)
    {
        if (reduceCountArray[x] == '\0'){
            break;
        }
        node *head = dataLinkList;
        node *tmp = dataLinkList;
        int y;

        for(y = 0; y < reduceCountArray[x]-1; y++) // minus one because error was caused without it
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
    printf("finished configuring buckets\n");
}

void finalReduce()
{
    printf("starting final reduce\n");
    int x;
    for (x = 0; x < reduces-1; x++)
    {
        if (buckets[x+1] != NULL && buckets[x] != NULL)
        {
            // if the last node in the bucket x has the same words as the first node in bucket x+1 then need to combine
            if (strcmp(buckets[x]->word,buckets[x+1]->next->word) == 0)
            {
                buckets[x]->count = buckets[x]->count +buckets[x+1]->next->count;
                // check if only one node in bucket
                if  (buckets[x+1]->next == buckets[x+1]){
                    buckets[x+1] = NULL;
                    moveBucketsToTheLeft(x+1); // need to move all buckets over 1 to the left
                }
                else
                {
                    buckets[x+1]->next = buckets[x+1]->next->next;
                }
                // reset index to 0 but since for loop will add +1 at the end of the loop must set x -1
                x = -1;
            }
        } 
    }
    printf("finished final reduce\n");
}

void moveBucketsToTheLeft(int startingBucket)
{
    printf("starting shifting buckets over\n");
    int x; 
    for(x = startingBucket; x < reduces-1; x++)
    {
        buckets[x] = buckets[x+1];   
    }
    buckets[reduces-1] = NULL; 
    printf("finished shifting buckets over\n");
}

void *reduce(void *bucketNumber)
{
    node *tail = buckets[(int)bucketNumber];
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
    buckets[(int)bucketNumber] = resultsPtr;
    return NULL;
}

void produceReduceThreadsAndWaitTillAllThreadsFinish()
{
    printf("tarting reduce threading\n");
    pthread_t reduceThread[reduces];
    int i;
    for (i = 0; i < reduces; i++)
    {
        reduceCountArray[i] = i;
        // not sure why you need (size_t) 
        if (buckets[i] && pthread_create(&reduceThread[i], NULL, reduce, (void *) (size_t) reduceCountArray[i] ) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < reduces; i++)
    {
        pthread_join(reduceThread[i], NULL);
    }
    printf("finished reduce threading\n");
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

void writeToFile()
{
    while (dataLinkListHead != NULL)
    {
        char *temp;
        char *message;
        int length = 0;
        if (strcmp(app, "-wordcount") == 0)
        {
            length = floor(log10(abs(dataLinkListHead->count))) + 1;
            temp = (char *)malloc(sizeof(char) + length + 1);
            sprintf(temp, "%d", dataLinkListHead->count);
            if (dataLinkListHead->next == NULL)
            {
                temp[strlen(temp)] = '\0';
            }
            else
            {
                temp[strlen(temp) + 1] = '\0';
                temp[strlen(temp)] = '\n';
            }

            message = (char *)malloc(sizeof(char) * length + 3 + strlen(dataLinkListHead->word));
            strcat(message, dataLinkListHead->word);

            strcat(message, " ");
            strcat(message, temp);
        }
        else
        {
            if (dataLinkListHead->next == NULL)
            {
                dataLinkListHead->word[strlen(dataLinkListHead->word)] = '\0';
            }
            else
            {
                dataLinkListHead->word[strlen(dataLinkListHead->word)] = '\n';
            }
            message = (char *)malloc(sizeof(char) * 1 + strlen(dataLinkListHead->word));
            strcat(message, dataLinkListHead->word);
        }
        if (write(outputFile, message, strlen(message)) < 0)
        {
            printf("Error writing to the file\n");
            exit(EXIT_FAILURE);
        }
        dataLinkListHead = dataLinkListHead->next;
    }
}

// //just used t o print out the output
// for (i = 0; i < mapsOrThreads - 1; i++)
// {
//     while (dataLinkListHead != NULL)
//     {
//         printf("(%s,%d)\n", dataLinkListHead->word, dataLinkListHead->count);
//         dataLinkListHead = dataLinkListHead->next;
//     }
// }