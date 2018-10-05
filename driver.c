#include "driver.h"
char *application;
char *implementation;
int numThreads;
int reduces;
int maps;
int input;
int output;
node *buckets;
node *arrayOfWords;
int totalMapsOrExtra;
int finalMapsOrExtra;
int keyCount;

//parese the input file and pull out words
void parseInputFile()
{
    char *temp = (char *)calloc(1, 1);
    temp[0] = '\0';
    char *singleChar = (char *)calloc(2, 1);
    singleChar[1] = '\0';

    while (read(input, &singleChar[0], 1) != 0)
    {
        if (validinput(singleChar[0]))
        {
            temp = realloc(temp, strlen(temp) + 2);
            strcat(temp, singleChar);
            continue;
        }
        if (strlen(temp) > 0 && validword(temp))
        {

            char *p;
            for (p = temp; *p; ++p)
                *p = tolower(*p);
            //add the words to our bucekts
            buckets = insertInput(buckets, temp);
            free(temp);
            temp = (char *)calloc(1, 1);
            temp[0] = '\0';
        }
        else
        {
            free(temp);
            temp = (char *)calloc(1, 1);
            temp[0] = '\0';
        }
    }
    if (strcmp(temp, "—") != 0 && strlen(temp) > 0)
    {
        char *p;
        for (p = temp; *p; ++p)
            *p = tolower(*p);
        buckets = insertInput(buckets, temp);
    }
    if (buckets == NULL)
    {
        printf("The input file was empty, or there was an error reading from it\n");
        exit(EXIT_FAILURE);
    }
    finalMapsOrExtra = keyCount < totalMapsOrExtra ? keyCount : totalMapsOrExtra;
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

int validword(char *temp)
{
    if (strcmp(temp, " ") != 0 && strcmp(temp, "\n") != 0 && strcmp(temp, "\t") != 0 && strcmp(temp, "\r") != 0 && strcmp(temp, "\0") != 0 && strcmp(temp, "'") != 0 && strcmp(temp, ".") != 0 && strcmp(temp, ",") != 0 && strcmp(temp, ";") != 0 && strcmp(temp, "!") != 0 && strcmp(temp, "-") != 0 && strcmp(temp, "—") != 0)
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
    if (!pointer)
    {
        totalMapsOrExtra = maps ? maps : numThreads;

        node *pointer = (node *)malloc(sizeof(node));
        pointer->word = (char *)malloc(sizeof(char) * strlen(temp) + 1);
        strcpy(pointer->word, temp);
        pointer->word[strlen(pointer->word)] = '\0';
        pointer->next = NULL;
        ++keyCount;
        return pointer;
    }
    node *tempData = (node *)malloc(sizeof(node));
    tempData->word = (char *)malloc(sizeof(char) * strlen(temp) + 1);
    strcpy(tempData->word, temp);
    tempData->word[strlen(tempData->word)] = '\0';
    tempData->next = pointer;
    pointer = tempData;
    ++keyCount;
    return pointer;
}

void generateArray()
{
    arrayOfWords = (node *)malloc(sizeof(node) * keyCount);
    int i = 0;
    while (buckets != NULL)
    {
        arrayOfWords[i] = *buckets;
        buckets = buckets->next;
        ++i;
    }
}

//drives the program
int main(int argc, char **argv)
{
    {
        // if we do not have 6 or 7 inputs we can not continue
        if (argc != 7 && argc != 6)
        {
            printf("Please make sure to have 6 or 7 inputs\n 7: ./mapred –-app [-wordcount, -sort] –-impl [-procs, -threads] --maps -[num_maps] –-reduces -[num_reduces] --input [-infile] –-output [-outfile]\n  6: ./mapred –-app [-wordcount, -sort] –-impl [-extra] --numthreads -[num] --input [-infile] –-output [-outfile]\n");
            exit(EXIT_FAILURE);
        }

        //before doing anything lets check if these files names exist/ are taken
        char dash = argc == 7 ? *argv[5] == '-' : *argv[4] == '-';
        input = argc == 7 ? open(++argv[5], O_RDONLY) : open(++argv[4], O_RDONLY);
        //if we couldn"t open the file let the user know it doesnt exist
        if (!dash || input < 0)
        {
            close(input);
            printf("We were unable to open your input file. Please check to make sure that file exists, or confirm the spelling.\n Please use the form -[file name]\n");
            exit(EXIT_FAILURE);
        }
        //lets create the output file
        dash = argc == 7 ? *argv[6] == '-' : *argv[5] == '-';
        output = argc == 7 ? open(++argv[6], O_RDWR | O_CREAT, 0777) : open(++argv[5], O_RDWR | O_CREAT, 0777);
        //if we could not create the file let them know and exit(EXIT_FAILURE)
        if (!dash || output < 0)
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
    }

    //read the input file and fill the buckets
    parseInputFile();
    //turn linked list into array
    generateArray();
    //call mapper
    mapper(arrayOfWords, keyCount, finalMapsOrExtra, reduces, application, output, implementation);
    return 0;
}