#include "process.h"

node **buckets;
int mapsOrThreads;
int reduces;
int outputFile;
char *app;
size_t size;

void initializeProcessMemory(node **newBuckets, int newMapsOrThreads, int newReduces, int output, char *application, int keyCount)
{
    //set up globals
    mapsOrThreads = newMapsOrThreads;
    reduces = newReduces;
    outputFile = output;
    app = malloc(strlen(application) + 1);
    strcpy(app, application);
    app[strlen(app)] = '\0';

    // set up sahred memory
    int shmid;
    key_t key;
    char *test;

    key = 1020183;
    shmid = shmget(key, sizeof(node *) * mapsOrThreads, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    // buckets = (node **)shmat(shmid, 0, 0);
    // node *test = buckets + sizeof(next);
    // next[0] = (node)malloc(sizeof(node));
    // next[0]->word = (char *)malloc(sizeof(char) * 5);
    // if (buckets == (void *)-1)
    // {
    //     perror("shmat");
    //     exit(EXIT_FAILURE);
    // }
    // int i;
    // size = sizeof(node);
    // for (i = 0; i < mapsOrThreads; ++i)
    // {
    //     int object = shmget(key, size, IPC_CREAT | 0666);
    //     if (object < 0)
    //     {
    //         perror("shmget");
    //         exit(EXIT_FAILURE);
    //     }
    //     buckets[i] = shmat(shmid, NULL, 0);
    //     if (buckets == (void *)-1)
    //     {
    //         perror("shmat");
    //         exit(EXIT_FAILURE);
    //     }
    // }
    // for (i = 0; i < mapsOrThreads; i++)
    // {
    //     while (newBuckets[i] != NULL)
    //     {
    //         int tempMem = shmget(key, sizeof(node), IPC_CREAT | 0666);
    //         if (tempMem < 0)
    //         {
    //             perror("shmget");
    //             exit(EXIT_FAILURE);
    //         }

    //         node *tempData = shmat(tempMem, NULL, 0);
    //         if (tempData == (void *)-1)
    //         {
    //             perror("shmat");
    //             exit(EXIT_FAILURE);
    //         }
    //         int tepMem2 = shmget(key, sizeof(strlen(newBuckets[i]->word) + 1), IPC_CREAT | 0666);
    //         if (tepMem2 < 0)
    //         {
    //             perror("shmget");
    //             exit(EXIT_FAILURE);
    //         }

    //         tempData->word = shmat(tempMem, NULL, 0);
    //         if (tempData->word == (void *)-1)
    //         {
    //             perror("shmat");
    //             exit(EXIT_FAILURE);
    //         }
    //         strcpy(tempData->word, newBuckets[i]->word);
    //         tempData->word[strlen(tempData->word)] = '\0';
    //         tempData->next = buckets[i];
    //         buckets[i] = tempData;
    //         newBuckets[i] = newBuckets[i]->next;
    //     }
    // }

    // for (i = 0; i < mapsOrThreads; i++)
    // {
    //     while (buckets[i] != NULL)
    //     {
    //         printf("%s\n", buckets[i]->word);
    //         buckets[i] = buckets[i]->next;
    //     }
    // }
    forkChildren();
    // system("ipcrm -M 1020183");
}

void forkChildren()
{
    int status;
    pid_t pid;
    int i;
    mapsOrThreads = 1;
    for (i = 0; i < mapsOrThreads; i++)
    {
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            fprintf(stderr, "Pipe Failed");
            exit(EXIT_FAILURE);
        }
        pid = fork();
        if (pid == -1)
        {
            printf("can't fork, error occured\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            printf("child process, pid = %u\n", getpid());
            execl("./processRunning", "map", itoa(mapsOrThreads), itoa(size), NULL);
            printf("We had an issue creating processes\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < mapsOrThreads; i++)
    {
        if (waitpid(pid, &status, 0) > 0)
        {
            if (WIFEXITED(status) && !WEXITSTATUS(status))
                printf("program execution successfull\n");

            else if (WIFEXITED(status) && WEXITSTATUS(status))
            {
                if (WEXITSTATUS(status) == 127)
                {
                    printf("execv failed\n");
                }
                else
                    printf("program terminated normally,"
                           " but returned a non-zero status\n");
            }
            else
                printf("program didn't terminate normally\n");
        }
        else
        {
            printf("waitpid() failed\n");
        }
        printf("parent process, pid = %u\n", getppid());

        exit(0);
    }
}

char *itoa(int val)
{
    char *str = calloc(10000, 1);
    sprintf(str, "%d", val);
    return str;
}
