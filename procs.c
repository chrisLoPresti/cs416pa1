#include "procs.h"

int initializeSHM()
{
    int shm_id = shmget(100, 2048, IPC_CREAT | 0600)
    if ( !shm_addr )
    {
        perror("shmget: ");
        exit(EXIT_FAILURE);
    }

    return shm_id;

} 

char* getSHMAddr(int shm_id)
{
    int shm_addr = shmat(shm_id, NULL, 0);
    if ( !shm_addr ) 
    {
        perror("shmat: ");
        exit(EXIT_FAILURE);
    }

    return shm_addr;
}