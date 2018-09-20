/* 
Notes from recitation on the assignment

***split by: spaces, tabs, new lines

***convert all words to lowercase 

example:
./mapred [-wordcount,-sort] [-procs, -threads] -4 -4 [-infile] [-outfile]

input file: this is a sample sentence to illustrate a simple point to you.

mission: read all of the words. mapper will split them up to different procs or threads

Now we see that there are 12 words in the sentence, split them by space or return character etc.

create key pairs as vectors:
map 1 => (this, 1) (is, 1) (a,1) 
map 2=> (simple, 1) (sentence, 1) (to, 1)
map 3=> (illustrate, 1) (a, 1) (simple, 1)
map 4=> (point, 1) (to, 1) (you, 1)

before we go to the reducer, shuffle! (sort all the words)

(a,1) (is, 1) (this, 1)
(sentence, 1) (simple, 1) (to, 1)
(a, 1) (illustrate, 1) (simple, 1)
(point, 1) (to, 1) (you, 1)

shuffle phase sorts =>

(a,1) (a,1) (illustrate, 1) (is, 1) (point, 1) (sentence, 1) (simple, 1) (simple, 1) (this, 1) (to, 1) (to, 1) (you, 1)

now reducer takes 3 words per and combines like terms=>

(a,2) (illustrate, 1)
(is, 1) (point, 1) (sentence, 1)
(simple, 2) (this, 1)
(to, 2) (you, 1)

do one last final combine phase to combine anything across reducer outputes
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
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
//a linked list of all the input data
static inputData *keys;
//count of the total number of inputs (words or numbers)
static int keysCount;

//helper function to parse the input file
void parseInputFile()
{
    char *temp = (char *)malloc(sizeof(char) * 2);
    char singleChar;
    while (read(input, &singleChar, 1) != 0)
    {
        if (singleChar != ' ' && singleChar != '\n' && singleChar != '\t' && singleChar != 0)
        {
            temp = realloc(temp, strlen(temp) + 2);
            temp[strlen(temp)] = singleChar;
            temp[strlen(temp) + 1] = '\0';
            continue;
        }
        //make a call to save the input key into memory
        if (strlen(temp) > 0)
        {
            for (char *p = temp; *p; ++p)
                *p = tolower(*p);
            keys = insertInput(keys, temp);
            temp = realloc(temp, 0);
        }
    }
    if (strlen(temp) > 0)
    {
        for (char *p = temp; *p; ++p)
            *p = tolower(*p);
        keys = insertInput(keys, temp);
    }
    free(temp);
    close(input);
}

//helper function to store all of the keys from the file
inputData *insertInput(inputData *keys, char *temp)
{
    if (keys == NULL)
    {
        keys = (inputData *)malloc(sizeof(inputData));
        if (keys == NULL)
        {
            printf("Error storing input word\n");
            exit(EXIT_FAILURE);
        }
        keys->word = (char *)malloc(sizeof(char) * strlen(temp) + 1);
        strcpy(keys->word, temp);
        keys->word[strlen(keys->word)] = '\0';
        keys->next = NULL;
        ++keysCount;
    }
    else
    {
        inputData *tempData = keys;
        while (tempData->next != NULL)
        {
            tempData = tempData->next;
        }
        tempData->next = (inputData *)malloc(sizeof(inputData));
        if (tempData->next == NULL)
        {
            printf("Error storing input word\n");
            exit(EXIT_FAILURE);
        }
        tempData->next->word = (char *)malloc(sizeof(char) * strlen(temp) + 1);
        strcpy(tempData->next->word, temp);
        tempData->next->word[strlen(tempData->next->word)] = '\0';
        tempData->next->next = NULL;
        ++keysCount;
    }
    return keys;
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

    if (argc == 7)
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
    else
    {
        // lets get the number of threads
        numThreads = atoi(++argv[4]);
        //make sure it is a valid number
        if (numThreads == 0)
        {
            printf("You entered an invalid --num_threads value of 0 or some form of characters.\n Please use the form -[num]\n");
            exit(EXIT_FAILURE);
        }
    }
    //parse the input file and collect all of the data from it
    parseInputFile();

    //not important, this is just to see whether or not we read in data succesfully
    while (keys != NULL)
    {
        printf("%s\n", keys->word);
        keys = keys->next;
    }
    printf("%d\n", keysCount);

    return 0;
}
