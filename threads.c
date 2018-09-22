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
    produceThreads();
}

void produceThreads()
{
    if (pthread_mutex_init(&dataLock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(EXIT_FAILURE);
    }
    pthread_t mapperThread;
    int i;
    for (i = 0; i < mapsOrThreads; i++)
    {
        if (pthread_create(&mapperThread, NULL, map, (void *)head->keys) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
        head = head->next;
    }
    pthread_join(mapperThread, NULL);
}

void *map(void *keys)
{
    inputData *newKeys = keys;
    while (newKeys != NULL)
    {
        printf("%s\n", newKeys->word);
        newKeys = newKeys->next;
    }
    return NULL;
}
