Gregory Katz gdk26
Chris LoPresti cpl62
Aryeh Pechet aap220
<br/>
MapReduce Framework
<br/>

Architectural Design
<br/>

Input phase: (driver.c)
<br/>

1. Verify input, making sure the request from the user is valid.
   a. Do the arguments make sense given the request?
   b. Is the input file valid / accessible?
   c. Is the output file valid / accessible? If it doesn’t exist, create it.
2. If the user request checks out, start reading from the file.
   a. Read from the file character by character, making sure each character is valid, building a word of potentially ‘unlimited’ length
   b. Once we have found a delimiter, take the word we have built and put it into a linked list, and update the number of total words that we have read
   c. Once we have reached the end f file, store our last word in the linked list and convert the entire linked list into an array of nodes so we can access each node via indexing, freeing the linked list nodes as we go
3. Give the mapper all of our arguments as well as the total number of words and the array of words that we created based off of the file
   <br/>

Mapper Phase: (mapred.c)
<br/>

1. Using the total number of words that we have, create POSIX shared memory using shmget in order to create an array of ints which will represent our word count
   a. Each index of this shared memory array directly relates to the same index in the node array
2. Now depending on the number of maps requested from the user, we do some math in order to make sure that the request makes sense. If we have 100 word and want 200 maps, that doesn’t make sense, so cut the number of maps down.
   a. We will also determine how many words to give to each thread or process that we will create
   b. We will also determine how many words the reducer will give to each thread or process that way when the reducer phase is called we can just start
3. Create threads or procs and tell each of them to go into shared memory and update the word count for each corresponding node index to 1, as there is going to be 1 of each word off the bat.
4. Once the threads or procs finish, the mapper will call sort (shuffle phase), which will merge sort our data
5. From here we will call the reducer
   <br/>

Reduce Phase: (mapred.c)
<br/>

1. The reducer now creates threads or process which will reduce the words that they have access to
2. Each thread or process will go through their words and if there are words that are the same, they will set the count of the left most word to be the total, while any identical word to the right will have its count set to 0
   a. Each thread or process has access to shared memory where it does the count updating
3. When we are done we will do a final reduce. This happens in the main thread or processes.
   a. We will only check words that were potentially overlapped between different threads or processes
4. From here we will go through the word array and shared memory count array at the same time.
   a. If the count of the word is above 0 we will print the word
   b. If we are using -wordcount we will print the word followed by its count
   c. If we are doing -sort we will print the word n number of times, where n = count
   d. After we print we clear the node from memory
   Extra Credit Phase: (mapred.c)
5. The same input phase happens and we still call mapper, however instead of doing the normal map reduce approach we do the extra credit approach
6. Mapper this time will call a function to dish out threads which will grab n number of words from the word array, and throw them into a joint BST
   a. This requires a mutex lock
7. When we insert a word it either goes to the right (if < the node word), to the left (if > the node word) or if the word equals the node word, we just update the node words count
   a. Whenever a none duplicate word is inserted the count is automatically set to 1
8. Once all threads are done, we go through the BST and print with the same criteria as the map reduce thread (same process for sort or wordcount)
   a. We clear the words as we go
9. IMPORTANT
   a. This process takes substantially more time then the shared memory index approach
   b. Because the BST requires making far more comparisons, the time goes from mere seconds to potentially 2-5 minutes for a 5mb file (roughly 600k words)

<br/>
	
Technical Difficulties
<br/>

We had to use our shared memory index count array approach as we were limited with shmget. We could not store pointers in shared memory, or even really 500k words. However we were able to store 500k+ integers with ease so it made more sense to have an array of counts where the indexes corresponded to the indexes of the words.

Another issue we had was for our BST extra credit approach. We had to implement a BST using while loops rather than recursion, as we were finding we were limited to 255 recursive calls, which hit us with an overflow error. When switching to a while loop we were able to do this no problem, however this BST approach does take a couple minutes rather then seconds when doing the shared memory index array approach.
