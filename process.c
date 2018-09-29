#include "process.h"

key_t key = 12345;
int shm_id;
int *shm_addr;
node *oneList;
node **bucketsPtr;
int totalKeys;
int keysperReduce;
int reducersNeeded;
int mapsNeeded;
char *app;
int outputFile;

void procsDriver(node **buckets, int keyCount, int finalMapsOrExtra, int reduces, char *application, int output)
{
    {
        outputFile = output;
        app = application;
        totalKeys = keyCount;
        keysperReduce = keyCount / reduces > 0 ? keyCount / reduces : 1;
        reducersNeeded = keysperReduce != 1 ? reduces : keyCount;
        mapsNeeded = finalMapsOrExtra;
        bucketsPtr = buckets;
    }

    createSharedMemory();

    oneList = sortProcs(oneList, keyCount, app);

    createMaps();
    createReduces();
    finalReducer();

    processWriteToFile();

    shmdt((void *)shm_addr);
    shmctl(shm_id, IPC_RMID, NULL);
}

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

void reduceProcs(int start, int end)
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
    int j, i;
    int finalEnd = end > totalKeys ? totalKeys : end;

    for (i = start; i < finalEnd; ++i)
    {
        if (*(shm_addr + i) == 0)
        {
            continue;
        }
        for (j = i + 1; j < finalEnd; ++j)
        {
            if (*(shm_addr + j) == 0)
            {
                continue;
            }
            if (strcmp(oneList[i].word, oneList[j].word) == 0)
            {
                *(shm_addr + i) += 1;
                *(shm_addr + j) = 0;
            }
        }
    }
}

void finalReducer()
{
    int i, j;
    for (i = 0; i < totalKeys; ++i)
    {
        if (*(shm_addr + i) == 0)
        {
            continue;
        }
        for (j = i + 1; j < totalKeys; ++j)
        {
            if (*(shm_addr + j) == 0)
            {
                continue;
            }
            if (strcmp(oneList[i].word, oneList[j].word) == 0)
            {
                *(shm_addr + i) += *(shm_addr + j);
                *(shm_addr + j) = 0;
            }
        }
    }
}

void createSharedMemory()
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

    oneList = (node *)malloc(sizeof(node) * totalKeys);
    int i = 0;
    int itterator = 0;
    for (i = 0; i < mapsNeeded; ++i)
    {
        while (bucketsPtr[i] != NULL)
        {
            oneList[itterator] = *bucketsPtr[i];
            bucketsPtr[i] = bucketsPtr[i]->next;
            *(shm_addr + itterator) = -i;
            ++itterator;
        }
    }
}

void createMaps()
{
    pid_t pid;
    int i;
    for (i = 0; i < mapsNeeded; ++i)
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
    }

    for (i = 0; i < mapsNeeded; ++i)
    {
        int returnStatus;
        waitpid(pid, &returnStatus, 0);
        if (returnStatus == 1)
        {
            printf("The child process terminated with an error!.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void createReduces()
{
    pid_t pid;
    int i;
    int extras = totalKeys % reducersNeeded <= 1 ? 0 : 1;
    int start = 0;
    int end = keysperReduce + extras;
    for (i = 0; i < reducersNeeded; ++i)
    {
        pid = fork();
        if (pid == 0)
        {
            reduceProcs(start, end);
            exit(EXIT_FAILURE);
        }
        else if (pid == -1)
        {
            printf("Error\n");
            exit(EXIT_FAILURE);
        }
        start += keysperReduce + extras + 1;
        end += end + extras + 1;
    }

    for (i = 0; i < mapsNeeded; ++i)
    {
        int returnStatus;
        waitpid(pid, &returnStatus, 0);
        if (returnStatus == 1)
        {
            printf("The child process terminated with an error!.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void processWriteToFile()
{
    int i;
    for (i = 0; i < totalKeys; ++i)
    {
        if (*(shm_addr + i) == 0)
        {
            continue;
        }
        processIndividualWrite(oneList[i].word, *(shm_addr + i));
    }
}

void processIndividualWrite(char *key, int count)
{
    char buf[1000];
    if (write(outputFile, key, strlen(key)) < 0)
    {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }

    if (write(outputFile, " ", 1) < 0)
    {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buf, "%d", count);
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