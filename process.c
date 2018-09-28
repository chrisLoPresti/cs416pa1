#include "process.h"

key_t keys = 123458;

void createSharedMemory(node **buckets, int keyCount, int finalMapsOrExtra, int charCount)
{
    //create shared memory to fit every single character, plus an int for a count, plus a comma for a delimiter
    //keyCount: we need that many delimiters, charCount: how many cahrs we have total
    int sharedMemKeys;
    int size = keyCount * sizeof(int) + keyCount * charCount + 1;
    if ((sharedMemKeys = shmget(keys, size, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget error.");
        exit(EXIT_FAILURE);
    }
    char *wordArray;
    if ((wordArray = (char *)shmat(sharedMemKeys, NULL, 0)) == (void *)-1)
    {
        shmdt((void *)wordArray);
        shmctl(sharedMemKeys, IPC_RMID, NULL);
        perror("shmat error");
        exit(EXIT_FAILURE);
    }

    int i, index;
    index = 0;
    node *prev;
    char buffer[1000];
    int indexes[finalMapsOrExtra][2];
    //go through our buckets and add each 'countword,' to shared memory
    for (i = 0; i < finalMapsOrExtra; ++i)
    {
        indexes[i][0] = strlen(wordArray);
        while (buckets[i] != NULL)
        {
            sprintf(buffer, "%d", buckets[i]->count);
            strcat(wordArray, buffer);
            strcat(wordArray, buckets[i]->word);
            strcat(wordArray, ",");
            prev = buckets[i];
            buckets[i] = buckets[i]->next;
            free(prev->word);
            free(prev);
            ++index;
        }
        indexes[i][1] = strlen(wordArray);
        // printf("%d %d\n", indexes[i][0], indexes[i][1]);
    }

    //convert our ints to strings to give to the child procs
    char shmKey[7];
    sprintf(shmKey, "%d", keys);

    int nDigits;

    nDigits = (int)floor(log10(abs(keyCount))) + 1;
    char keyStr[nDigits + 1];
    sprintf(keyStr, "%d", keyCount);

    nDigits = (int)floor(log10(abs(size))) + 1;
    char sizeStr[nDigits + 1];
    sprintf(sizeStr, "%d", size);

    pid_t pid;
    for (i = 0; i < finalMapsOrExtra; i++)
    {

        //tell the child proc what indecies in the shm string they need to worry about
        int startlength = (int)floor(log10(abs(indexes[i][0]))) + 1;
        char *startStr = (char *)malloc(sizeof(startlength + 1));
        sprintf(startStr, "%d", indexes[i][0]);
        int endlength = nDigits = (int)floor(log10(abs(indexes[i][1]))) + 1;
        char *endStr = (char *)malloc(sizeof(endlength + 1));
        sprintf(endStr, "%d", indexes[i][1]);

        pid = fork();
        if (pid == 0)
        {
            //create child procs
            execl("processRunning", keyStr, startStr, endStr, sizeStr, shmKey, NULL);
            printf("Could not create child process\n");
            exit(EXIT_FAILURE);
        }
        //if we couldnt create let the user know and clear the shared mem
        else if (pid == -1)
        {
            printf("Error\n");
            shmdt((void *)wordArray);
            shmctl(sharedMemKeys, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }
        // printf("%s %s %s %s \n", keyStr, startStr, endStr, sizeStr);
        free(startStr);
        free(endStr);
    }
    //wait for  child procsto finish
    for (i = 0; i < finalMapsOrExtra; ++i)
    {
        int returnStatus;
        // Parent process waits here for child to terminate.
        waitpid(pid, &returnStatus, 0);
        //if return is not 0 it had an error
        if (returnStatus == 1)
        {
            printf("The child process terminated with an error!.\n");
            //clear shared mem
            shmdt((void *)wordArray);
            shmctl(sharedMemKeys, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }
    }
    //clear shared mem
    shmdt((void *)wordArray);
    shmctl(sharedMemKeys, IPC_RMID, NULL);
}

//    for (i = 0; i < keyCount; ++i)
//     {
//         printf("%s %d\n", wordArray[i], countArray[i]);
//     }