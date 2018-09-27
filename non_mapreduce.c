#include "threads.h"

node **buckets; // items in buckets are stored as a circular link list
node *dataLinkList;
int totalThreads; // Note total threads is equal to the number of bucket
int totalNodes; // total node in the link list
char *app; // what kind of sort is needed
int *reduceCountArray; // this int array is used to store how many nodes are needed for each bucket in the reduce stage
int outputFile; // output file


void nonMapReduceDriver()
{
    // combine all node from the bucket in to link list.
    deconstructBucketInToLinkList();

    // set all buckets pointers to null
    nullAllBucketsPointers();

    // sort link list
    dataLinkList = sort(dataLinkList, app);

    // generate how many node will go in to each bucket 
    generateHowManyNodesEachBucketWillContainForNonMapReduce();

    // configure buckets to contain the correct amount of nodes
    configureBucketsToContainCorrectNumberOfNodesForNonMapReduce();

    // start mapping and reducing
    produceNonMapReduceThreadsAndWaitTillAllThreadsFinish();

    // combine all adjoining nodes that match 
    finalReduceForNonMapReduce();

    // print results to file
    passBucketToWriteToFileForNonMapReduce();
}

void printBucketsForNonMapReduce()
{
    printf("Starting printing buckets\n");
    int x;
    for (x = 0; x < totalThreads; x++)
    {
        printf("Printing data in bucket %d:\n",x);
        if (buckets[x] == NULL)
        {
            break;
        }
        node *tmp = buckets[x]->next;
        while (tmp != buckets[x])
        {
            printf("%s, %d\n" , tmp->word, tmp->count);
            tmp = tmp->next;
        }
        printf("%s, %d\n" , tmp->word, tmp->count);
    }
    printf("Ending printing buckets\n");

}

void nullAllBucketsPointers()
{
    int x;
    for (x = 0; x < totalThreads; x++)
    {
        buckets[x] = NULL;
    }

}

void deconstructBucketInToLinkList()
{
    dataLinkList = NULL;
    node *tail = dataLinkList;
    int x;
    for (x = 0; x < totalThreads; x++)
    {
        if (buckets[x] == NULL)
        {
            continue;
        }
        if (dataLinkList == NULL)
        {
            dataLinkList = buckets[x];
            tail = dataLinkList;
            while (tail->next != NULL)
            {
                tail = tail->next;    
            }
        }
        else
        {
            tail->next = buckets[x];
            while (tail->next != NULL)
            {
                tail = tail->next;    
            }
        }
    }
}

void generateHowManyNodesEachBucketWillContainForNonMapReduce()
{
    printf("generating each bucket number\n");
    // create array which will store how many nodes each reduce thread will handle
    reduceCountArray = malloc( (sizeof(int) * totalThreads) + 1 * sizeof('\0'));
    reduceCountArray[totalThreads] = '\0';

    int x;
    int reduceCountArrayIndex = 0;
    // zero out all memory
    for(x = 0; x < totalThreads; x++)
    {
        reduceCountArray[x] = 0;
    }

    for(x = 0; x < totalThreads; x++)
    {
        if (reduceCountArrayIndex == totalThreads)
        {
            reduceCountArrayIndex = 0;
        }
        reduceCountArray[reduceCountArrayIndex] = reduceCountArray[reduceCountArrayIndex] + 1;
        reduceCountArrayIndex++;
    }
    printf("finished generating each bucket number\n");
}

void configureBucketsToContainCorrectNumberOfNodesForNonMapReduce()
{
    printf("starting configuring buckets\n");
    int x;
    for (x=0; x < totalThreads; x++)
    {
        if (reduceCountArray[x] == 0){
            continue;
        }
        node *head = dataLinkList;
        node *tmp = dataLinkList;
        int y;

        for(y = 0; y < reduceCountArray[x]-1; y++) // minus one because error was caused without it
        {
            tmp = tmp->next;
        }
        // move main pointer to next starting position 
        dataLinkList = tmp->next; 
        

        // create circular link list in bucket
        buckets[x] = head;
        tmp->next = buckets[x];
        buckets[x] = tmp;
    } 
    printf("finished configuring buckets\n");
}

void *nonMapReduce(void *bucketNumber)
{
    node *tail = buckets[(int)bucketNumber];
    node *head = tail->next;
    tail->next = NULL; // changes input list from circular to linear
    node *results = NULL;
    node *resultsPtr = NULL;

    while (head != NULL)
    {
        if (resultsPtr == NULL)
        {
            if (head->count == 0)
            {
                head->count = 1;
            }
            results = head;
            resultsPtr = head;
            head = head->next;

        }
        else
        {
            if (strcmp(resultsPtr->word, head->word) == 0)
            {
                if (head->count == 0)
                {
                    head->count = 1;
                }
                resultsPtr->count = resultsPtr->count + head->count;
                node *tmp = head->next;
                free(head->word);
                free(head);
                head = tmp;
            }
            else
            {
                if (head->count == 0)
                {
                    head->count = 1;
                }
               resultsPtr->next = head;
               resultsPtr = resultsPtr->next;
               head = head->next;
            } 
        }
    }
    
    resultsPtr->next = results;
    buckets[(int)bucketNumber] = resultsPtr;
    return NULL;
}

void produceNonMapReduceThreadsAndWaitTillAllThreadsFinish()
{
    printf("starting reduce threading\n");
    pthread_t reduceThread[totalThreads];
    int i;
    for (i = 0; i < totalThreads; i++)
    {
        reduceCountArray[i] = i;
        // not sure why you need (size_t) 
        if (buckets[i] && pthread_create(&reduceThread[i], NULL, nonMapReduce, (void *) (size_t) reduceCountArray[i] ) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < totalThreads; i++)
    {
        pthread_join(reduceThread[i], NULL);
    }
    printf("finished reduce threading\n");
}

void finalReduceForNonMapReduce()
{
    printf("starting final reduce\n");
    int x;
    for (x = 0; x < totalThreads-1; x++)
    {
        if (buckets[x+1] != NULL && buckets[x] != NULL)
        {
            // if the last node in the bucket x has the same words as the first node in bucket x+1 then need to combine
            if (strcmp(buckets[x]->word,buckets[x+1]->next->word) == 0)
            {
                buckets[x]->count = buckets[x]->count +buckets[x+1]->next->count;
                // check if only one node in bucket
                if  (buckets[x+1]->next == buckets[x+1]){
                    buckets[x+1] = NULL;
                    moveBucketsToTheLeftForNonMapReduce(x+1); // need to move all buckets over 1 to the left
                }
                else
                {
                    buckets[x+1]->next = buckets[x+1]->next->next;
                }
                // reset index to 0 but since for loop will add +1 at the end of the loop must set x -1
                x = -1;
            }
        } 
    }
    printf("finished final reduce\n");
}

void moveBucketsToTheLeftForNonMapReduce(int startingBucket)
{
    //printf("starting shifting buckets over\n");
    int x; 
    for(x = startingBucket; x < totalThreads-1; x++)
    {
        buckets[x] = buckets[x+1];   
    }
    buckets[totalThreads-1] = NULL; 
    //printf("finished shifting buckets over\n");
}

void passBucketToWriteToFileForNonMapReduce()
{
    //printf("staring to print to file \n");
    int x;
    for (x = 0; x < totalThreads; x++)
    {
        //printf("%d\n", x);
        if (buckets[x] == NULL)
        {
            break;
        }
        node *head = buckets[x]->next;
        if (buckets[x] == head)
        {
            head-> next = NULL;
        }
        else
        {
            buckets[x]-> next = NULL;
        }
        // node *dataLinkListHead = head;
        // while (dataLinkListHead != NULL)
        // {
        //     printf("node %s,%d\n", dataLinkListHead->word,dataLinkListHead->count);
        //     dataLinkListHead = dataLinkListHead->next;
        // }

        writeToFileForNonMapReduce(head);
    }
}

void writeToFileForNonMapReduce(node *head)
{

    node *dataLinkListHead = head;

    while (dataLinkListHead != NULL)
    {
        if (strcmp(app, "-wordcount") == 0)
        {

            if (write(outputFile, dataLinkListHead->word, strlen(dataLinkListHead->word)) < 0)
            {
                printf("Error writing to the file\n");
                exit(EXIT_FAILURE);
            }
            if (write(outputFile, ", ", strlen(", ")) < 0)
            {
                printf("Error writing to the file\n");
                exit(EXIT_FAILURE);
            }
            // todo change to allow any size int
            char result[50]; 
            int num = dataLinkListHead->count; 
            sprintf(result, "%d", num); 

            int length = floor(log10(abs(dataLinkListHead->count))) + 1;
            if (write(outputFile, result, length) < 0)
            {
                printf("Error writing to the file\n");
                exit(EXIT_FAILURE);
            }

            if (write(outputFile, "\n", strlen("\n")) < 0)
            {
                printf("Error writing to the file\n");
                exit(EXIT_FAILURE);
            }

        }
        else
        {
            if (write(outputFile, dataLinkListHead->word, strlen(dataLinkListHead->word)) < 0)
            {
                printf("Error writing to the file\n");
                exit(EXIT_FAILURE);
            }   
            if (write(outputFile, "\n", strlen("\n")) < 0)
            {
                printf("Error writing to the file\n");
                exit(EXIT_FAILURE);
            } 
        }
        dataLinkListHead = dataLinkListHead->next;
    }
}