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

//driver to pull in parameters and call other functions
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

    // oneList = sortProcs(oneList, keyCount, app);
    node *temp = (node *)malloc(sizeof(node) * keyCount);
    printf("sorting...\n");
    oneList = myMergeSort(oneList, temp, keyCount, app);
    printf("done sorting...\n");

    createMaps();
    createReduces();
    if (reducersNeeded != 1)
    {
        finalReducer();
    }

    processWriteToFile();

    shmdt((void *)shm_addr);
    shmctl(shm_id, IPC_RMID, NULL);
}

//goes into shared memory and updates the total for each word to be 1
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

//goes into shared memory and increments the number of times a word is present
//if it is proceeded by words that are the same
//words that are the same have their count set to 0
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
        for (j = i + 1; j < finalEnd; ++j)
        {
            if (strcmp(oneList[i].word, oneList[j].word) == 0)
            {
                *(shm_addr + i) += 1;
                *(shm_addr + j) = 0;
                i = j - 1;
            }
            else
            {
                i = j - 1;
                break;
            }
        }
    }
}

//goes through shared memory and makes sure overlaps are accounted for
void finalReducer()
{
    int i, j;
    for (i = 0; i < totalKeys; ++i)
    {
        for (j = i + 1; j < totalKeys; ++j)
        {
            if (strcmp(oneList[i].word, oneList[j].word) == 0)
            {
                *(shm_addr + i) += *(shm_addr + j);
                *(shm_addr + j) = 0;
                i = j - 1;
            }
            else
            {
                i = j - 1;
                break;
            }
        }
    }
}

//creates shared memory and our array of nodes
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

//function that forks to create children that map
void createMaps()
{
    pid_t pid[mapsNeeded];
    int i;
    for (i = 0; i < mapsNeeded; ++i)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            mapProcs(i);
            exit(EXIT_FAILURE);
        }
        else if (pid[i] == -1)
        {
            printf("Error\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < mapsNeeded; ++i)
    {
        int returnStatus;
        waitpid(pid[i], &returnStatus, 0);
        // printf("The child process %d finished.\n", pid[i]);
        if (returnStatus == 1)
        {
            printf("The child process terminated with an error!.\n");
            exit(EXIT_FAILURE);
        }
    }
}

//function that forks to create children that reduce
void createReduces()
{
    pid_t pid[reducersNeeded];
    int i;
    int extras = totalKeys % reducersNeeded <= 1 ? 0 : 1;
    int start = 0;
    int end = keysperReduce + extras;
    for (i = 0; i < reducersNeeded; ++i)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            reduceProcs(start, end);
            exit(EXIT_FAILURE);
        }
        else if (pid[i] == -1)
        {
            printf("Error\n");
            exit(EXIT_FAILURE);
        }
        start += keysperReduce + extras + 1;
        end += end + extras + 1;
    }

    for (i = 0; i < reducersNeeded; ++i)
    {
        int returnStatus;
        waitpid(pid[i], &returnStatus, 0);
        // printf("The child process %d finished.\n", pid[i]);
        if (returnStatus == 1)
        {
            printf("The child process %d finished.\n", pid[i]);
            printf("The child process terminated with an error!.\n");
            exit(EXIT_FAILURE);
        }
    }
}

//go through the array and send each nodes string and count to processIndividualWrite
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

//writes each key and count to the file
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