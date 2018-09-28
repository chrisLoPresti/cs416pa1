#include "processRunning.h"

node *buckets;
int keys;
int start;
int end;
int size;
int size;
key_t shmKey;

int main(int argc, char **argv)
{
    keys = atoi(argv[0]);
    // start = atoi(argv[1]);
    // end = atoi(argv[2]);
    // size = atoi(argv[3]);
    // shmKey = (key_t)atoi(argv[4]);

    // int sharedMemKeys;
    // if ((sharedMemKeys = shmget(shmKey, size, IPC_CREAT | 0666)) < 0)
    // {
    //     perror("shmget error.");
    //     exit(EXIT_FAILURE);
    // }

    // char *wordArray;

    // if ((wordArray = (char *)shmat(sharedMemKeys, NULL, 0)) == (void *)-1)
    // {
    //     shmdt((void *)wordArray);
    //     shmctl(sharedMemKeys, IPC_RMID, NULL);
    //     perror("shmat error");
    //     exit(EXIT_FAILURE);
    // }

    printf("%d \n", keys);

    return 0;
}