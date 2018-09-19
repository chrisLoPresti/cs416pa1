#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* 
sample input 

mapred –-app [wordcount, sort] –-impl [procs, threads] --maps num_maps –-reduces num_reduces --input infile –-output outfile
*/

static char *application;
static char *implementation;
static int reduces;
static int maps;
static int input;
static int output;

int main(int argc, char **argv)
{
    // if we do not have 7 inputs we can not continue
    if (argc != 7)
    {
        printf("Please make sure to have 7 inputs\n mapred –-app [wordcount, sort] –-impl [procs, threads] --maps num_maps –-reduces num_reduces --input infile –-output outfile\n");
        exit(EXIT_FAILURE);
    }

    //before doing anything lets check if these files names exist/ are taken
    input = open(argv[5], O_RDONLY);
    //if we couldn"t open the file let the user know it doesnt exist
    if (input < 0)
    {
        printf("We were unable to open your input file. Please check to make sure that file exists, or confirm the spelling.\n");
        exit(EXIT_FAILURE);
    }
    //lets create the output file
    output = open(argv[6], O_RDWR | O_CREAT, 0777);
    //if we could not create the file let them know and exit(EXIT_FAILURE)
    if (output < 0)
    {
        printf("We were unable to open your output file. Either there was an error creating it, or it already exists and we do not have permission to open it.\n");
        exit(EXIT_FAILURE);
    }

    //get the application type (wordcount or sort)
    application = (char *)(malloc(sizeof(char) * strlen(argv[1]) + 1));
    strcpy(application, argv[1]);
    application[strlen(application)] = '\0';
    //make sure the application type is valid
    if (strcmp(application, "wordcount") != 0 && strcmp(application, "sort") != 0)
    {
        printf("You entered an invalid --app.\nValid options=>\nwordcount\nsort\nPlease try again with a valid option.\n");
        exit(EXIT_FAILURE);
    }

    //get the implementation method(procs, threads, extra)
    implementation = (char *)(malloc(sizeof(char) * strlen(argv[2]) + 1));
    strcpy(implementation, argv[2]);
    implementation[strlen(implementation)] = '\0';
    //make sure the implementation type is valid
    if (strcmp(implementation, "procs") != 0 && strcmp(implementation, "threads") != 0 && strcmp(implementation, "extra") != 0)
    {
        printf("You entered an invalid --impl.\nValid options=>\nprocs\nthreads\nextra\nPlease try again with a valid option.\n");
        exit(EXIT_FAILURE);
    }

    //lets get the number of maps
    maps = atoi(argv[3]);
    //make sure it is a valid number
    if (maps == 0)
    {
        printf("You entered an invalid --maps value of 0 or some form of characters.\n");
        exit(EXIT_FAILURE);
    }

    //lets get the number of reduces
    reduces = atoi(argv[4]);
    //make sure it is a valid number
    if (reduces == 0)
    {
        printf("You entered an invalid --reduces value of 0 or some form of characters.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}