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

Mapper is the first thing to be called after gathering our input keys. Mapper will recieve an array of nodes.

The driver.c file will produce an array: node \* and send it to mapper
<br/>
<br/>

```c
struct node
{
    char *word;

} typedef node;
```

<br/>
<br/>

Sort will also use this same array of nodes, as well as our function to write output to our file.
