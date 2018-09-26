#include "threads.h"

node **buckets;
node *dataLinkList;
node *dataLinkListHead;
pthread_mutex_t dataLock;
int mapsOrThreads;
int reduces;
int outputFile;
int totalKeys;
char *app;

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
    dataLinkList = sort(dataLinkList);

    dataLinkListHead = dataLinkList;
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

    dataLinkList = sort(dataLinkList, app);
    dataLinkListHead = dataLinkList;
    writeToFile();
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
<<<<<<< HEAD
            length = floor(log10(abs(dataLinkListHead->count))) + 1;
            temp = (char *)malloc(sizeof(char) + length + 1);
            sprintf(temp, "%d", dataLinkListHead->count);
            if (dataLinkListHead->next == NULL)
            {
                temp[strlen(temp)] = '\0';
            }
            else
=======
            length = floor(log10(abs(combinedDataHead->count))) + 1;
            temp = (char *)malloc(sizeof(char) + length + 2);
            sprintf(temp, "%d", combinedDataHead->count);
            if (combinedDataHead->next != NULL)
>>>>>>> master
            {
                temp[strlen(temp) + 1] = '\0';
                temp[strlen(temp)] = '\n';
            }
<<<<<<< HEAD
            message = (char *)malloc(sizeof(char) * length + 3 + strlen(dataLinkListHead->word));
            strcat(message, dataLinkListHead->word);
=======
            message = (char *)calloc(strlen(temp) + strlen(combinedDataHead->word) + 2, 1);

            strcat(message, combinedDataHead->word);
>>>>>>> master
            strcat(message, " ");
            strcat(message, temp);
        }
        else
        {
<<<<<<< HEAD
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
=======
            if (combinedDataHead->next != NULL)
            {
                message = (char *)calloc(strlen(combinedDataHead->word) + 2, 1);
                strcat(message, combinedDataHead->word);
                int length = strlen(message);
                message[length] = '\n';
                message[length + 1] = '\0';
            }
            else
            {
                message = (char *)calloc(strlen(combinedDataHead->word) + 1, 1);
                strcat(message, combinedDataHead->word);
            }
>>>>>>> master
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