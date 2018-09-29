#include "mapred.h"
char *application;
char *implementation;
int numThreads;
int reduces;
int maps;
int input;
int output;
node **buckets;
int totalMapsOrExtra;
int finalMapsOrExtra;
int keyCount;
//parese the input file and pull out words
void parseInputFile()
{
    char *temp = (char *)calloc(1, 1);
    char *singleChar = (char *)calloc(2, 1);

    int itterator = 0;
    while (read(input, &singleChar[0], 1) != 0)
    {
        if (validinput(singleChar[0]))
        {
            temp = realloc(temp, strlen(temp) + 2);
            strcat(temp, singleChar);
            continue;
        }
        if (strlen(temp) > 0)
        {
            char *p;
            for (p = temp; *p; ++p)
                *p = tolower(*p);
            //add the words to our bucekts
            buckets[itterator] = insertInput(buckets[itterator], temp);
            free(temp);
            temp = (char *)calloc(1, 1);
            ++itterator;
            if (itterator == totalMapsOrExtra)
            {
                itterator = 0;
            }
        }
    }
    if (strlen(temp) > 0)
    {
        char *p;
        for (p = temp; *p; ++p)
            *p = tolower(*p);
        buckets[itterator] = insertInput(buckets[itterator], temp);
    }
    if (buckets[0] == NULL)
    {
        printf("The input file was empty, or there was an error reading from it\n");
        exit(EXIT_FAILURE);
    }
    finalMapsOrExtra = keyCount < totalMapsOrExtra ? itterator + 1 : totalMapsOrExtra;
    free(temp);
    close(input);
}

//checks to make sure the character is not a delimiter
int validinput(char character)
{
    if (character != ' ' && character != '\n' && character != '\t' && character != '\r' && character != '\0' && character != 0 && character != '.' && character != ',' && character != ';' && character != '!' && character != '-')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//adds a full word to our buckets
node *insertInput(node *pointer, char *temp)
{
    node *tempData = (node *)malloc(sizeof(node));
    tempData->word = (char *)malloc(sizeof(char) * strlen(temp) + 1);
    strcpy(tempData->word, temp);
    tempData->word[strlen(tempData->word)] = '\0';
    tempData->next = pointer;
    pointer = tempData;
    tempData->count = 0;
    ++keyCount;
    return pointer;
}

//creates our buckets based off of the number of requested maps
void initializeBuckets()
{
    totalMapsOrExtra = maps ? maps : numThreads;

    buckets = (node **)malloc(sizeof(node *) * totalMapsOrExtra);
    int i;
    for (i = 0; i < totalMapsOrExtra; ++i)
    {
        buckets[i] = malloc(sizeof(node));
        buckets[i] = NULL;
    }
}

//removes any buckets that were not used
void *cleanBuckets()
{
    int i;
    for (i = finalMapsOrExtra; i < totalMapsOrExtra; ++i)
    {
        free(buckets[i]);
    }
    return NULL;
}

//drives the program
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
        if (maps <= 0)
        {
            printf("You entered an invalid --maps value of <=0 or some form of characters.\n Please use the form -[num]\n");
            exit(EXIT_FAILURE);
        }

        //lets get the number of reduces
        reduces = atoi(++argv[4]);
        //make sure it is a valid number
        if (reduces <= 0)
        {
            printf("You entered an invalid --numReduces value of <=0 or some form of characters.\n Please use the form -[num]\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (argc == 6 && strcmp(implementation, "-extra") == 0)
    {
        // lets get the number of threads
        numThreads = atoi(++argv[3]);
        //make sure it is a valid number
        if (numThreads <= 0)
        {
            printf("You entered an invalid --num_threads value of <=0 or some form of characters.\n Please use the form -[num]\n");
            exit(EXIT_FAILURE);
        }
    }
    //if the input stream is invalid let them know
    else
    {
        printf("Please make sure to have 6 or 7 inputs\n 7: ./mapred –-app [-wordcount, -sort] –-impl [-procs, -threads] --maps -[num_maps] –-reduces -[num_reduces] --input [-infile] –-output [-outfile]\n  6: ./mapred –-app [-wordcount, -sort] –-impl [-extra] --numthreads -[num] --input [-infile] –-output [-outfile]\n");
        exit(EXIT_FAILURE);
    }

    //create our buckets
    initializeBuckets();
    //read the input file and fill the buckets
    parseInputFile();
    //create a thread to clean the buckets we did not use

    if (strcmp(implementation, "-threads") == 0)
    {
        threading_driver(buckets, finalMapsOrExtra, reduces, output, application, keyCount);

        pthread_t cleaner;
        if (pthread_create(&cleaner, NULL, cleanBuckets, NULL) != 0)
        {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
        pthread_join(cleaner, NULL);
    }
    else
    {
        procsDriver(buckets, keyCount, finalMapsOrExtra, reduces, application, output);
    }

    return 0;
}

/* 
this is code you can use to test to see if the buckets have information

    printf("(we need %d, buckets and will remove any excess)\n", finalMapsOrExtra);
    int i = 0;
    for (i = 0; i < finalMapsOrExtra; i++)
    {
        while (buckets[i] != NULL)
        {
            printf("(%s,%d)\n", buckets[i]->word, buckets[i]->count);
            buckets[i] = buckets[i]->next;
        }
    }
    */