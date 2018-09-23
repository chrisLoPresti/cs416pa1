#include "threads.h"

node **buckets;
node *combinedData;
node *combinedDataHead;
pthread_mutex_t dataLock;
int mapsOrThreads;
int reduces;
int outputFile;
int totalKeys;
char *app;

void initializeThreadMemory(node **newBuckets, int newMapsOrThreads, int newReduces, int output, char *application)
{
    buckets = newBuckets;
    mapsOrThreads = newMapsOrThreads;
    reduces = newReduces;
    combinedData = NULL;
    outputFile = output;
    app = malloc(strlen(application) + 1);
    strcpy(app, application);
    app[strlen(app)] = '\0';
    produceThreadMaps();
}

void produceThreadMaps()
{
    if (pthread_mutex_init(&dataLock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(EXIT_FAILURE);
    }
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

    combinedData = sort(combinedData);
    combinedDataHead = combinedData;
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

    if (!combinedData)
    {
        combinedData = (node *)malloc(sizeof(node));
        combinedData = startptr;
    }
    else
    {
        lastptr->next = combinedData;
        combinedData = startptr;
    }
    pthread_mutex_unlock(&dataLock);

    return NULL;
}

void writeToFile()
{
    while (combinedDataHead != NULL)
    {
        char *temp;
        char *message;
        int length = 0;
        if (strcmp(app, "-wordcount") == 0)
        {
            length = floor(log10(abs(combinedDataHead->count))) + 1;
            temp = (char *)malloc(sizeof(char) + length + 1);
            sprintf(temp, "%d", combinedDataHead->count);
            if (combinedDataHead->next == NULL)
            {
                temp[strlen(temp)] = '\0';
            }
            else
            {
                temp[strlen(temp)] = '\n';
            }
            message = (char *)malloc(sizeof(char) * length + 3 + strlen(combinedDataHead->word));
            strcat(message, combinedDataHead->word);
            strcat(message, " ");
            strcat(message, temp);
        }
        else
        {
            if (combinedDataHead->next == NULL)
            {
                combinedDataHead->word[strlen(combinedDataHead->word)] = '\0';
            }
            else
            {
                combinedDataHead->word[strlen(combinedDataHead->word)] = '\n';
            }
            message = (char *)malloc(sizeof(char) * 1 + strlen(combinedDataHead->word));
            strcat(message, combinedDataHead->word);
        }

        if (write(outputFile, message, strlen(message)) < 0)
        {
            printf("Error writing to the file\n");
            exit(EXIT_FAILURE);
        }
        combinedDataHead = combinedDataHead->next;
    }
}

// //just used t o print out the output
// for (i = 0; i < mapsOrThreads - 1; i++)
// {
//     while (combinedDataHead != NULL)
//     {
//         printf("(%s,%d)\n", combinedDataHead->word, combinedDataHead->count);
//         combinedDataHead = combinedDataHead->next;
//     }
// }