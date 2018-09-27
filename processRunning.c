#include <stdio.h>
#include "processRunning.h"

node **buckets;

int main(int argc, char **argv)
{
    printf("%s %s %s\n", argv[0], argv[1], argv[2]);
    // char *temp = calloc(5, 1);
    int shmid;
    key_t key;
    size_t totalBuckets = atoi(argv[1]);
    size_t size = atoi(argv[2]);

    key = 1020183;
    shmid = shmget(key, size, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    buckets = shmat(shmid, NULL, 0);
    if (buckets == (void *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    printf("HELLO BIATCHHHHHH %zu %s\n", totalBuckets, buckets[0]->word);
    return 0;
}