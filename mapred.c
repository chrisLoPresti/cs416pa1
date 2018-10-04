#include "mapred.h"
//key variable to access POSIX shared memory
key_t key = 12345;
//id and pointer to point to POSIX memory
int shm_id;
int *shm_addr;
//list if our keys (array of nodes)
node *oneList;
//how many keys we have in our array
int totalKeys;
//how many keys we will hand to each reducers
int keysperReduce;
//how many reducer threads or procs we actually need (lets say you asked for too many, we make it so we give you an ammount that makes sense)
int reducersNeeded;
//same concept applies for how many mapping threads or procs you requested
int mapsNeeded; //how many keys we will hand to each reducers
int keysperMap;
//how many keys we will hand to each reducers
int keysperReduce;
// -wordcount or -sort
char *app;
// -procs or -threads
char *impl;
//our file to write to
int outputFile;

//mapper is called from the driver.c file
//can not name it map because map is a reserved function name for c
//mapp takes in arguements and creates threads or procs to generate key pairs
//once key pairs are assigned, we sort the oneList of data
//from here we call the reducer, had to name it reducer because reduce is a reserved function name in c
void mapper(node *buckets, int keyCount, int finalMapsOrExtra, int reduces, char *application, int output, char *implementtion)
{
    //assign global variables
    {
        oneList = buckets;
        outputFile = output;
        app = application;
        totalKeys = keyCount;
        keysperReduce = keyCount / reduces > 0 ? keyCount / reduces : 1;
        reducersNeeded = keysperReduce != 1 ? reduces : keyCount;
        mapsNeeded = finalMapsOrExtra;
        keysperMap = keyCount / mapsNeeded > 0 ? keyCount / mapsNeeded : 1;
        impl = implementtion;
    }
    //create POSIX shared memory
    createSharedMemory();
    //this determines whether or not we use threads or procs depending on implementation (impl)
    if (strcmp(impl, "-procs") == 0)
    {
        generateKeysProcs();
    }
    else if (strcmp(impl, "-threads") == 0)
    {
        generateKeysThreads();
    }

    node *temp = (node *)malloc(sizeof(node) * keyCount);
    oneList = myMergeSortDriver(oneList, temp, keyCount, app);
    //call reducer which will create threads or procs to perform reducing actions
    reducer();
    //write output to file
    processWriteToFile();
    //de-attached from shared name and clear it
    shmdt((void *)shm_addr);
    shmctl(shm_id, IPC_RMID, NULL);
}

//creates shared memory array of indecies corresponding to our data
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
}

//function that forks to create children that create key pair associations
void generateKeysProcs()
{
    //we will create forks that will go to our shared memory, where they will set the associated words count to 1
    int extras = totalKeys % mapsNeeded <= 1 ? 0 : totalKeys % mapsNeeded;
    int start = 0;
    int end = 0;
    if (extras && extras - 1 >= 0)
    {
        --extras;
        end += keysperReduce + 1;
    }
    else
    {
        end += keysperReduce;
    }
    pid_t pid[mapsNeeded];
    int i;

    for (i = 0; i < mapsNeeded; ++i)
    {
        int *info = malloc(8);
        info[0] = 0 + start;
        info[1] = 0 + end;
        pid[i] = fork();
        if (pid[i] == 0)
        {
            generateKeyPair((void *)info);
            exit(EXIT_SUCCESS);
        }
        else if (pid[i] == -1)
        {
            printf("Error\n");
            exit(EXIT_FAILURE);
        }

        if (extras && extras - 1 >= 0)
        {
            --extras;
            start = end;
            end += keysperReduce + 1;
        }
        else
        {
            start = end;
            end += keysperReduce;
        }
    }

    for (i = 0; i < mapsNeeded; ++i)
    {
        int returnStatus;
        waitpid(pid[i], &returnStatus, 0);
        if (returnStatus == 1)
        {
            printf("The child process terminated with an error!.\n");
            exit(EXIT_FAILURE);
        }
    }
}

//function that threads to create children that create key pair associations
void generateKeysThreads()
{
    //we will create threads that will go to our shared memory, where they will set the associated words count to 1
    pthread_t pThread[mapsNeeded];
    int i;
    int extras = totalKeys % mapsNeeded <= 1 ? 0 : totalKeys % mapsNeeded;
    int start = 0;
    int end = 0;
    if (extras && extras - 1 >= 0)
    {
        --extras;
        end += keysperReduce + 1;
    }
    else
    {
        end += keysperReduce;
    }
    for (i = 0; i < mapsNeeded; ++i)
    {
        int *info = malloc(8);
        info[0] = 0 + start;
        info[1] = 0 + end;
        if (pthread_create(&pThread[i], NULL, generateKeyPair, (void *)info) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
        start += keysperReduce + extras + 1;
        if (extras && extras - 1 >= 0)
        {
            --extras;
            start = end;
            end += keysperReduce + 1;
        }
        else
        {
            start = end;
            end += keysperReduce;
        }
    }

    for (i = 0; i < mapsNeeded; i++)
    {
        pthread_join(pThread[i], NULL);
    }
}

//goes into shared memory and updates the total for each word to be 1
void *generateKeyPair(void *info)
{
    //we have an index array in shared memory corresponding to the indecies of our data
    //this allows us to use minimal shared memory while still keeping track of ourdata safely amongst processes and threads
    int start = ((int *)info)[0];
    int end = ((int *)info)[1];
    int finalEnd = end > totalKeys ? totalKeys : end;

    if (strcmp(impl, "-procs") == 0)
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
    }
    int c;
    for (c = start; c < finalEnd; ++c)
    {
        *(shm_addr + c) = 1;
    }
    return NULL;
}

//function that forks to create children that reduce
void reducer()
{
    if (strcmp(impl, "-procs") == 0)
    {
        generateReducersProcs();
    }
    else if (strcmp(impl, "-threads") == 0)
    {
        generateReducersThreads();
    }
    //perform a final reduce only if we requested more then 1 reducer
    if (reducersNeeded != 1)
    {
        finalReducer();
    }
}

//will create x number of processes to reduce a chunck of data
void generateReducersProcs()
{
    pid_t pid[reducersNeeded];
    int i;
    int extras = totalKeys % reducersNeeded <= 1 ? 0 : totalKeys % reducersNeeded;
    int start = 0;
    int end = 0;
    if (extras && extras - 1 >= 0)
    {
        --extras;
        end += keysperReduce + 1;
    }
    else
    {
        end += keysperReduce;
    }
    for (i = 0; i < reducersNeeded; ++i)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            int *x = malloc(8);
            x[0] = 0 + start;
            x[1] = 0 + end;
            reduceCombined((void *)x);
            exit(EXIT_SUCCESS);
        }
        else if (pid[i] == -1)
        {
            printf("Error\n");
            exit(EXIT_FAILURE);
        }
        if (extras && extras - 1 >= 0)
        {
            --extras;
            start = end;
            end += keysperReduce + 1;
        }
        else
        {
            start = end;
            end += keysperReduce;
        }
    }

    for (i = 0; i < reducersNeeded; ++i)
    {
        int returnStatus;
        waitpid(pid[i], &returnStatus, 0);
        if (returnStatus == 1)
        {
            printf("The child process %d finished.\n", pid[i]);
            printf("The child process terminated with an error!.\n");
            exit(EXIT_FAILURE);
        }
    }
}

//will create x number of threads to reduce a chunck of data
void generateReducersThreads()
{
    pthread_t pThread[reducersNeeded];
    int i;
    int extras = totalKeys % reducersNeeded <= 1 ? 0 : totalKeys % reducersNeeded;
    int start = 0;
    int end = 0;
    if (extras && extras - 1 != 0)
    {
        --extras;
        end += keysperReduce + 1;
    }
    else
    {
        end += keysperReduce;
    }
    for (i = 0; i < reducersNeeded; ++i)
    {
        int *x = malloc(8);
        x[0] = 0 + start;
        x[1] = 0 + end;
        if (pthread_create(&pThread[i], NULL, reduceCombined, (void *)x) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
        if (extras && extras - 1 != -1)
        {
            --extras;
            start = end;
            end += keysperReduce + 1;
        }
        else
        {
            start = end;
            end += keysperReduce;
        }
    }

    for (i = 0; i < reducersNeeded; i++)
    {
        pthread_join(pThread[i], NULL);
    }
}

//goes into shared memory and increments the number of times a word is present
//if it is proceeded by words that are the same
//words that are the same have their count set to 0
void *reduceCombined(void *x)
{

    int start = ((int *)x)[0];
    int end = ((int *)x)[1];

    if (strcmp(app, "-procs") == 0)
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
            else
            {
                i = j - 1;
                break;
            }
        }
    }
    return NULL;
}

//goes through shared memory and makes sure overlaps are accounted for
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
            else
            {
                i = j - 1;
                break;
            }
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
        free(oneList[i].word);
    }
    free(oneList);
    close(outputFile);
}

//writes each key and count to the file
void processIndividualWrite(char *key, int count)
{
    if (strcmp(app, "-sort") == 0)
    {
        int i;
        for (i = 0; i < count; ++i)
        {
            if (write(outputFile, key, strlen(key)) < 0)
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
    }
    else
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
}