#include "threads.h"

bucket *buckets;
bucket *head;
int mapsOrThreads;
int reduces;

void initializeMemory(bucket *newBuckets, int newMapsOrThreads, int newReduces)
{

    buckets = newBuckets;
    head = newBuckets;
    mapsOrThreads = newMapsOrThreads;
    reduces = newReduces;

    int i;
    for (i = 0; i < mapsOrThreads; i++)
    {
        while (head->keys != NULL)
        {
            printf("%s bucket: %d\n", head->keys->word, head->id);
            head->keys = head->keys->next;
        }
        printf("done with all keys in bucket %d\n", head->id);
        head = head->next;
    }
}
