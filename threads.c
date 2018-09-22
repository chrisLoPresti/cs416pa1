#include "threads.h"

bucket *buckets;
bucket *head;
node *combinedData;
pthread_mutex_t dataLock;
int mapsOrThreads;
int reduces;

void initializeMemory(bucket *newBuckets, int newMapsOrThreads, int newReduces)
{
    buckets = newBuckets;
    head = newBuckets;
    mapsOrThreads = newMapsOrThreads;
    reduces = newReduces;
    combinedData = NULL;
    produceThreads();
}

void produceThreads()
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
        if (pthread_create(&mapperThread[i], NULL, map, (void *)head->keys) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
        head = head->next;
    }
    for (int i = 0; i < mapsOrThreads; i++)
    {
        pthread_join(mapperThread[i], NULL);
    }

    //not needed - just to print key pairs
    while (combinedData != NULL)
    {
        printf("(%s,%d)\n", combinedData->word, combinedData->count);
        combinedData = combinedData->next;
    }
}

void *map(void *keys)
{
    inputData *newKeys = keys;
    while (newKeys != NULL)
    {
        pthread_mutex_lock(&dataLock);
        if (combinedData == NULL)
        {
            combinedData = (node *)malloc(sizeof(node));
            combinedData->word = (char *)malloc(sizeof(char) * strlen(newKeys->word) + 1);
            strcpy(combinedData->word, newKeys->word);
            combinedData->word[strlen(combinedData->word)] = '\0';
            combinedData->count = 1;
            combinedData->next = NULL;
        }
        else
        {
            node *temp = (node *)malloc(sizeof(node));
            temp->word = (char *)malloc(sizeof(char) * strlen(newKeys->word) + 1);
            strcpy(temp->word, newKeys->word);
            temp->word[strlen(temp->word)] = '\0';
            temp->count = 1;
            temp->next = combinedData;
            combinedData = temp;
        }
        newKeys = newKeys->next;
        pthread_mutex_unlock(&dataLock);
    }
    return NULL;
}
