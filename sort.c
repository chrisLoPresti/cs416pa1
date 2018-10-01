#include "sort.h"

//this merge sort is for threads, as they will be sending in a singley linked list
void myMergeSortDriverThreads(node **mapResults, char *app)
{
    //if the list is length < 2 return it because we cant sort it
    if (mapResults == NULL || *mapResults == NULL || (*mapResults)->next == NULL)
    {
        return;
    }

    //else split the list into two lists
    node *temp1, *temp2;
    temp1 = NULL;
    temp2 = NULL;

    splitMapResults(*mapResults, &temp1, &temp2);
    myMergeSortDriverThreads(&temp1, app);
    myMergeSortDriverThreads(&temp2, app);
    *mapResults = myMergeSortThreads(temp1, temp2, app);
}

void splitMapResults(node *mapResults, node **temp1, node **temp2)
{
    if (mapResults == NULL || mapResults->next == NULL)
    {
        *temp1 = mapResults;
        *temp2 = NULL;
        return;
    }
    node *first = mapResults;
    node *second = mapResults->next;

    while (second != NULL)
    {
        second = second->next;
        if (second != NULL)
        {
            first = first->next;
            second = second->next;
        }
    }
    //temp 2 now points to 1 node passed the midpoint
    *temp1 = mapResults;
    *temp2 = first->next;
    first->next = NULL;
}

node *myMergeSortThreads(node *temp1, node *temp2, char *app)
{
    if (temp1 == NULL)
    {
        // printf("temp1: %s temp2: %s\n", "NULL", temp2->word);
        return temp2;
    }
    if (temp2 == NULL)
    {
        // printf("temp1: %s temp2: %s\n", temp1->word, "NULL");
        return temp1;
    }

    // printf("temp1: %s temp2: %s\n", temp1->word, temp2->word);

    node *finalSortedList = NULL;
    if (strcmp(app, "-sort") == 0 && atoi(temp1->word) <= atoi(temp2->word))
    {
        finalSortedList = temp1;
        finalSortedList->next = myMergeSortThreads(temp1->next, temp2, app);
    }
    else if (strcmp(app, "-sort") == 0 && atoi(temp1->word) > atoi(temp2->word))
    {
        finalSortedList = temp2;
        finalSortedList->next = myMergeSortThreads(temp1, temp2->next, app);
    }
    else if (strcmp(app, "-wordcount") == 0 && strcmp(temp1->word, temp2->word) <= 0)
    {
        finalSortedList = temp1;
        finalSortedList->next = myMergeSortThreads(temp1->next, temp2, app);
    }
    else if (strcmp(app, "-wordcount") == 0 && strcmp(temp1->word, temp2->word) > 0)
    {
        finalSortedList = temp2;
        finalSortedList->next = myMergeSortThreads(temp1, temp2->next, app);
    }

    return finalSortedList;
}

//this merge sort is used for processes, as they will be sending in an array of nodes
node *
myMergeSortDriverProcs(node *nodeArray, node *tmpArray, int totalKeys, char *app)
{
    myMergeSortProcs(app, nodeArray, tmpArray, 0, totalKeys - 1);
    return nodeArray;
}

void myMergeSortProcs(char *app, node *nodeArray, node *tmpArray, int left, int right)
{
    int mid;
    if (right > left)
    {
        mid = (right + left) / 2;
        myMergeSortProcs(app, nodeArray, tmpArray, left, mid);
        myMergeSortProcs(app, nodeArray, tmpArray, mid + 1, right);
        myMergeProcs(app, nodeArray, tmpArray, left, mid + 1, right);
    }
}

void myMergeProcs(char *app, node *nodeArray, node *tmpArray, int left, int mid, int right)
{
    int i, farLeft, count, tmpArray_pos;
    farLeft = mid - 1;
    tmpArray_pos = left;
    count = right - left + 1;

    while ((left <= farLeft) && (mid <= right))
    {
        if (strcmp(app, "-wordcount") == 0 && strcmp(nodeArray[left].word, nodeArray[mid].word) <= 0)
        {
            tmpArray[tmpArray_pos] = nodeArray[left];
            tmpArray_pos = tmpArray_pos + 1;
            left = left + 1;
        }
        else if (strcmp(app, "-sort") == 0 && atoi(nodeArray[left].word) <= atoi(nodeArray[mid].word))
        {
            tmpArray[tmpArray_pos] = nodeArray[left];
            tmpArray_pos = tmpArray_pos + 1;
            left = left + 1;
        }
        else
        {
            tmpArray[tmpArray_pos] = nodeArray[mid];
            tmpArray_pos = tmpArray_pos + 1;
            mid = mid + 1;
        }
    }
    while (left <= farLeft)
    {
        tmpArray[tmpArray_pos] = nodeArray[left];
        left = left + 1;
        tmpArray_pos = tmpArray_pos + 1;
    }
    while (mid <= right)
    {
        tmpArray[tmpArray_pos] = nodeArray[mid];
        mid = mid + 1;
        tmpArray_pos = tmpArray_pos + 1;
    }
    for (i = 0; i < count; i++)
    {
        nodeArray[right] = tmpArray[right];
        right = right - 1;
    }
}
