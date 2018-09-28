#include "process.h"

key_t key = 12345;
int shm_id;
int *shm_addr;
node *oneList;
int totalKeys;

void mapProcs(int i)
{
    shm_id = shmget(key, sizeof(int) * totalKeys, IPC_CREAT | 0600);
    if (!shm_id)
    {
        perror("shmget: ");
        exit(EXIT_FAILURE);
    }

    shm_addr = shmat(shm_id, NULL, 0);
    if (!shm_addr)
    {
        perror("shmat: ");
        exit(EXIT_FAILURE);
    }
    int c;
    for (c = 0; c < totalKeys; ++c)
    {
        if (*(shm_addr + c) == -i)
        {
            *(shm_addr + c) = 1;
        }
    }
}

void syncLists()
{
    int c;
    for (c = 0; c < totalKeys; ++c)
    {
        oneList[c].count = *(shm_addr + c);
    }

    //this will print and confirm our mapping
    for (c = 0; c < totalKeys; ++c)
    {
        printf("%s %d\n", oneList[c].word, oneList[c].count);
    }
}

void createSharedMemory(node **buckets, int keyCount, int finalMapsOrExtra, int charCount)
{
    totalKeys = keyCount;

    shm_id = shmget(key, sizeof(int) * keyCount, IPC_CREAT | 0600);
    if (!shm_id)
    {
        perror("shmget: ");
        exit(EXIT_FAILURE);
    }

    shm_addr = shmat(shm_id, NULL, 0);
    if (!shm_addr)
    {
        perror("shmat: ");
        exit(EXIT_FAILURE);
    }

    oneList = (node *)malloc(sizeof(node) * keyCount);
    int i = 0;
    int itterator = 0;
    for (i = 0; i < finalMapsOrExtra; ++i)
    {
        while (buckets[i] != NULL)
        {
            oneList[itterator] = *buckets[i];
            buckets[i] = buckets[i]->next;
            *(shm_addr + itterator) = -i;
            ++itterator;
        }
    }

    pid_t pid;
    for (i = 0; i < finalMapsOrExtra; ++i)
    {
        pid = fork();
        if (pid == 0)
        {
            mapProcs(i);
            exit(EXIT_FAILURE);
        }
        else if (pid == -1)
        {
            printf("Error\n");
            exit(EXIT_FAILURE);
        }
        // printf("%d %d\n", indexes[i][0], indexes[i][1]);
    }

    for (i = 0; i < finalMapsOrExtra; ++i)
    {
        int returnStatus;
        waitpid(pid, &returnStatus, 0);
        if (returnStatus == 1)
        {
            printf("The child process terminated with an error!.\n");
            exit(EXIT_FAILURE);
        }
    }
    syncLists();
    shmdt((void *)shm_addr);
    shmctl(shm_id, IPC_RMID, NULL);
}

//    for (i = 0; i < keyCount; ++i)
//     {
//         printf("%s %d\n", wordArray[i], countArray[i]);
//     }