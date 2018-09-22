#include "mapred.h"
//holds the values -wordcount or -sort
static char *application;
//holds the values -procs,-threads or -extra
static char *implementation;
//if we chose -extra, get the humber of threads
static int numThreads;
//else get the number of reduces
static int reduces;
//and the number of maps
static int maps;
//our input file
static int input;
//our output file
static int output;
//buckets to split our input keys
static bucket *buckets;
static bucket *head;

//helper function to parse the input file
void parseInputFile()
{
    //temp is going to be our string that continues to be built from valid characters
    char *temp = (char *)malloc(sizeof(char) * 2); //singleChar is what we will recieve from the input file
    char singleChar;
    //while we are able to read a character ...
    while (read(input, &singleChar, 1) != 0)
    {
        //check to make sure the character is valid, if not its considered a key seperator
        if (singleChar != ' ' && singleChar != '\n' && singleChar != '\t' && singleChar != '\r' && singleChar != 0)
        {
            //resize the string to hold one more character and append the singleChar to it
            temp = realloc(temp, strlen(temp) + 1);
            temp[strlen(temp)] = singleChar;
            temp[strlen(temp) + 1] = '\0';
            continue;
        }
        //make a call to save the input key into memory IFF the key is a non-empyt string
        if (strlen(temp) > 0)
        {
            // //we verified that the string is a valid key, so convert it to lowercase and insert it to memory
            for (char *p = temp; *p; ++p)
                *p = tolower(*p);
            buckets = insertInput(buckets, temp);
            // //reset temp so we can build a fresh word
            temp = realloc(NULL, 0);
            buckets = buckets->next;
        }
    }
    //we read EOF so the loop broke
    if (strlen(temp) > 0)
    {
        for (char *p = temp; *p; ++p)
            *p = tolower(*p);
        buckets = insertInput(buckets, temp);
    }
    //if there are no keys the file was empty
    if (head->keys == NULL)
    {
        printf("The input file was empty, or there was an error reading from it\n");
        exit(EXIT_FAILURE);
    }
    // free temp as we no longer need it
    free(temp);
    //close the file as we no longer need it
    close(input);
}

//helper function to store all of the keys from the file
bucket *insertInput(bucket *buckets, char *temp)
{
    inputData *tempData = (inputData *)malloc(sizeof(inputData));
    tempData->word = (char *)malloc(sizeof(char) * strlen(temp) + 1);
    strcpy(tempData->word, temp);
    tempData->word[strlen(tempData->word)] = '\0';
    tempData->next = buckets->keys;
    buckets->keys = tempData;
    return buckets;
}

//helper function to create our buckets
void initializeBuckets()
{
    buckets = (bucket *)malloc(sizeof(bucket));
    buckets->keys = NULL;
    buckets->id = 0;
    head = buckets;
    int totalMapsOrExtra = maps ? maps : numThreads;
    int i;
    for (i = 1; i < totalMapsOrExtra; i++)
    {
        buckets->next = (bucket *)malloc(sizeof(bucket));
        buckets = buckets->next;
        buckets->id = i;
        buckets->keys = NULL;
    }
    buckets->next = head;
    buckets = head;
}

int main(int argc, char **argv)
{
    // if we do not have 6 or 7 inputs we can not continue
    if (argc != 7 && argc != 6)
    {
        printf("Please make sure to have 6 or 7 inputs\n 7: ./mapred –-app [-wordcount, -sort] –-impl [-procs, -threads] --maps -[num_maps] –-reduces -[num_reduces] --input [-infile] –-output [-outfile]\n  6: ./mapred –-app [-wordcount, -sort] –-impl [-extra] --numthreads -[num] --input [-infile] –-output [-outfile]\n");
        exit(EXIT_FAILURE);
    }

    //before doing anything lets check if these files names exist/ are taken
    input = argc == 7 ? open(++argv[5], O_RDONLY) : open(++argv[4], O_RDONLY);
    //if we couldn"t open the file let the user know it doesnt exist
    if (input < 0)
    {
        close(input);
        printf("We were unable to open your input file. Please check to make sure that file exists, or confirm the spelling.\n Please use the form -[file name]\n");
        exit(EXIT_FAILURE);
    }
    //lets create the output file
    output = argc == 7 ? open(++argv[6], O_RDWR | O_CREAT, 0777) : open(++argv[5], O_RDWR | O_CREAT, 0777);
    //if we could not create the file let them know and exit(EXIT_FAILURE)
    if (output < 0)
    {
        printf("We were unable to open your output file. Either there was an error creating it, or it already exists and we do not have permission to open it.\n Please use the form -[file name]\n");
        exit(EXIT_FAILURE);
    }

    //get the application type (wordcount or sort)
    application = (char *)(malloc(sizeof(char) * strlen(argv[1]) + 1));
    strcpy(application, argv[1]);
    application[strlen(application)] = '\0';
    //make sure the application type is valid
    if (strcmp(application, "-wordcount") != 0 && strcmp(application, "-sort") != 0)
    {
        printf("You entered an invalid --app.\nValid options=>\n-wordcount\n-sort\nPlease try again with a valid option.\n");
        exit(EXIT_FAILURE);
    }

    //get the implementation method(procs, threads, extra)
    implementation = (char *)(malloc(sizeof(char) * strlen(argv[2]) + 1));
    strcpy(implementation, argv[2]);
    implementation[strlen(implementation)] = '\0';
    //make sure the implementation type is valid
    if (strcmp(implementation, "-procs") != 0 && strcmp(implementation, "-threads") != 0 && strcmp(implementation, "-extra") != 0)
    {
        printf("You entered an invalid --impl.\nValid options=>\n-procs\n-threads\n-extra\nPlease try again with a valid option.\n");
        exit(EXIT_FAILURE);
    }

    //validate to make sure we have the correct input amount matching our implementation
    if (argc == 7 && (strcmp(implementation, "-procs") == 0 || strcmp(implementation, "-threads") == 0))
    {
        //lets get the number of maps
        maps = atoi(++argv[3]);
        //make sure it is a valid number
        if (maps == 0)
        {
            printf("You entered an invalid --maps value of 0 or some form of characters.\n Please use the form -[num]\n");
            exit(EXIT_FAILURE);
        }

        //lets get the number of reduces
        reduces = atoi(++argv[4]);
        //make sure it is a valid number
        if (reduces == 0)
        {
            printf("You entered an invalid --numThreads value of 0 or some form of characters.\n Please use the form -[num]\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (argc == 6 && strcmp(implementation, "-extra") == 0)
    {
        // lets get the number of threads
        numThreads = atoi(++argv[3]);
        //make sure it is a valid number
        if (numThreads == 0)
        {
            printf("You entered an invalid --num_threads value of 0 or some form of characters.\n Please use the form -[num]\n");
            exit(EXIT_FAILURE);
        }
    }
    //if the input stream is invalid let them know
    else
    {
        printf("Please make sure to have 6 or 7 inputs\n 7: ./mapred –-app [-wordcount, -sort] –-impl [-procs, -threads] --maps -[num_maps] –-reduces -[num_reduces] --input [-infile] –-output [-outfile]\n  6: ./mapred –-app [-wordcount, -sort] –-impl [-extra] --numthreads -[num] --input [-infile] –-output [-outfile]\n");
        exit(EXIT_FAILURE);
    }
    //create our buckets so we can use them to split up our file
    initializeBuckets();
    //parse the input file and collect all of the data from it
    parseInputFile();
    int totalMapsOrExtra = maps ? maps : numThreads;

    initializeMemory(head, totalMapsOrExtra, reduces, output);
    return 0;
}
