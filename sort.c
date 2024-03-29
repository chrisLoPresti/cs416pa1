#include "sort.h"

//this merge sort is generic and happens at the end of mapping
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
        if (strcmp(app, "-wordcount") == 0 && (atoi(nodeArray[left].word) != 0 || strcmp(nodeArray[left].word, "0") == 0) && (atoi(nodeArray[mid].word) != 0 || strcmp(nodeArray[mid].word, "0") == 0))
        {
            if (atoi(nodeArray[left].word) <= atoi(nodeArray[mid].word))
            {
                tmpArray[tmpArray_pos] = nodeArray[left];
                tmpArray_pos = tmpArray_pos + 1;
                left = left + 1;
            }
            else if (atoi(nodeArray[left].word) > atoi(nodeArray[mid].word))
            {
                tmpArray[tmpArray_pos] = nodeArray[mid];
                tmpArray_pos = tmpArray_pos + 1;
                mid = mid + 1;
            }
        }
        else if (strcmp(app, "-wordcount") == 0 && (atoi(nodeArray[left].word) != 0 || strcmp(nodeArray[left].word, "0") == 0) && atoi(nodeArray[mid].word) == 0)
        {
            tmpArray[tmpArray_pos] = nodeArray[left];
            tmpArray_pos = tmpArray_pos + 1;
            left = left + 1;
        }
        else if (strcmp(app, "-wordcount") == 0 && atoi(nodeArray[left].word) == 0 && (atoi(nodeArray[mid].word) != 0 || strcmp(nodeArray[mid].word, "0") == 0))
        {
            tmpArray[tmpArray_pos] = nodeArray[mid];
            tmpArray_pos = tmpArray_pos + 1;
            mid = mid + 1;
        }
        else if (strcmp(app, "-wordcount") == 0 && strcmp(nodeArray[left].word, nodeArray[mid].word) <= 0)
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
