#include "procs.h"

node **buckets;
int mapsOrThreads;
int reduces;
int outputFile;
int totalNodes;
char *app;
int *reduceCountArray;
int key = 100;

int startProcessing(node **newBuckets, int newMapsOrThreads, int newReduces, int output, char *application)
{
    // initialize all information 
    buckets = newBuckets;
    mapsOrThreads = newMapsOrThreads;
    reduces = newReduces; 
    outputFile = output;
    app = malloc(strlen(application) + 1);
    strcpy(app, application);

	int* signals = initializeSHM(mapsOrThreads*sizeof(int));
    pid_t pid;
    int bucketForProcessToHandle = -1;
    for ( int i = 0; i < mapsOrThreads; i++ )
    {
        pid = fork();
        if ( pid == 0 )
        {
            bucketForProcessToHandle = i;
            break;
        }
    }

    map_proc(buckets[i]);

    if (bucketForProcessToHandle == -1 )
    {
        waitpid();
    }
        

	return 0;
	
	
}

char* initializeSHM(int alloc)
{
    int shm_id = shmget(key, alloc, IPC_CREAT | 0600);
    if ( !shm_id )
    {
        perror("shmget: ");
        exit(EXIT_FAILURE);
    }
	key++;
    return getSHMAddr(shm_id);

}

char* getSHMAddr(int shm_id)
{
    char* shm_addr = shmat(shm_id, NULL, 0);
    if ( !shm_addr ) 
    {
        perror("shmat: ");
        exit(EXIT_FAILURE);
    }

    return shm_addr;
}

void map_proc(node *bucket)
{
    node *startptr = bucketData;
    node *lastptr;
    while (bucketData != NULL)
    {
        bucketData->count = 1;
        lastptr = bucketData;
        bucketData = bucketData->next;
    }
}