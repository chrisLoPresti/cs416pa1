#include "sort.h"

/*
    Bubble sort is used for sorting. 
    Sorting takes place on each node's char *word and not int count.
    strcmp is used for comparing each nodes char *word.
    To test the sorting method run testSort() function. 

*/

node *sort(node *head, char *app)
{
    node *current;
    node *ptr;
    int numbers = strcmp(app, "-sort") == 0 ? 1 : 0;
    for (current = head; current != NULL; current = current->next)
    {
        for (ptr = head; ptr != NULL; ptr = ptr->next)
        {
            if (!numbers && strcmp(ptr->word, current->word) > 0)
            {
                int count = current->count;
                char *tempWord = current->word;

                current->count = ptr->count;
                current->word = ptr->word;

                ptr->count = count;
                ptr->word = tempWord;
            }
            else if (atoi(ptr->word) > atoi(current->word))
            {
                int count = current->count;
                char *tempWord = current->word;

                current->count = ptr->count;
                current->word = ptr->word;

                ptr->count = count;
                ptr->word = tempWord;
            }
        }
    }

    return head;
}

node *myMergeSortDriver(node *nodeArray, node *tmpArray, int totalKeys, char *app)
{
    myMergeSort(app, nodeArray, tmpArray, 0, totalKeys - 1);
    return nodeArray;
}

void myMergeSort(char *app, node *nodeArray, node *tmpArray, int left, int right)
{
    int mid;
    if (right > left)
    {
        mid = (right + left) / 2;
        myMergeSort(app, nodeArray, tmpArray, left, mid);
        myMergeSort(app, nodeArray, tmpArray, mid + 1, right);
        myMerge(app, nodeArray, tmpArray, left, mid + 1, right);
    }
}

void myMerge(char *app, node *nodeArray, node *tmpArray, int left, int mid, int right)
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
