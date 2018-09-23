# CS 416 Assignment 1

## Getting Started

First clone the repo to your machine.
<br/>
Then go to github and create a new branch to work in.
<br/>
Once you have the branch, go to your terminal and type:
<br/>
<br/>
git fetch --all
<br/>
<br/>
This will pull in all newly created branches to your local repo.
<br/>
Now type:
<br/>
<br/>
git checkout [repo name]
<br/>
<br/>
This will place you in that branch so you can start working.

## Testing Code

First type "make" or "Make" to complie the code using the Makefile that is provided.
<br/>
Then type:
<br/>
<br/>
./mapred [-wordcount,-sort][-procs, -threads] -[num_maps] -[num_reduces][-infile][-outfile]
<br/>
<br/>
Or if using the extra credit:
<br/>
<br/>
./mapred [-wordcount,-sort][-extra] -[num_threads][-infile][-outfile]
<br/>
<br/>
Here is a working example with the provided code:
<br/>
./mapred -wordcount -threads -4 -4 -input.txt -results.txt
<br/>
<br/>
The code should run!

## Contracts --important

The mapreduc function will create buckets based off of the number of maps
(or threads we need to make if using extra credit)

The mapred.c file will produce buckets: node \*\*
<br/>
<br/>

```c
struct node
{
    char *word;
    int count;
    struct node *next;

} typedef node;
```

<br/>
<br/>
Which will be populated with inputData from the file: node *

<br/>
<br/>

```c
struct node
{
    char *word;
    int count;
    struct node *next;

} typedef node;
```

<br/>
<br/>

From here if we are implementing threads,
the mapred.c file will give the whole list of buckets to the threads.c file.
<br/>

The threads.c file will create threads and hand each thread a list of inputData
from each bucket.
<br/>
The threads will be mapping input data with a count creating: node\*
<br/>

```c
struct node
{
    char *word;
    int count;
    struct node *next;

} typedef node;
```

<br/>

The output from each thread will be combined together.
Once we are done combinding information, we will sort if using the sort.c file.
Sort.c takes in a list of combinedData: node\*
<br/>

```c
struct node
{
    char *word;
    int count;
    struct node *next;

} typedef node;
```
