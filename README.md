# CS 416 Assignment 1

## Getting Started

First clone the repo to your machine.
<br/>
Then go to github and create a new branch to work in.
<br/>
Once you have the branch, go to your terminal and type:
<br/>
git fetch --all
<br/>
This will pull in all newly created branches to your local repo.
<br/>
Now type: <br/>
git checkout [repo name]
<br/>
This will place you in that branch so you can start working.

## Testing Code

First type "make" or "Make" to complie the code using the Makefile that is provided.
<br/>
Then type:
<br/>
./mapred [-wordcount,-sort][-procs, -threads] -[num_maps] -[num_reduces] [-infile][-outfile]
<br/>
Or if using the extra credit:
<br/>
./mapred [-wordcount,-sort][-extra] -[num_threads] [-infile][-outfile]
<br/>
<br/>
Here is a working example with the provided code:
<br/>
./mapred -wordcount -procs -4 -4 -sort.csv -results.txt
<br/>
<br/>
The code should run!
