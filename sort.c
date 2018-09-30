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

// node *sortProcs(node *array, int length, char *app)
// {
//     // printf("sorting...\n");
//     int numbers = strcmp(app, "-sort") == 0 ? 1 : 0;
//     int i, j;
//     for (i = 0; i < length; ++i)
//     {
//         for (j = i; j < length; ++j)
//         {
//             if (!numbers && strcmp(array[j].word, array[i].word) < 0)
//             {
//                 char *tempWord = array[i].word;
//                 array[i].word = array[j].word;
//                 array[j].word = tempWord;
//             }
//             else if (atoi(array[j].word) < atoi(array[i].word))
//             {
//                 char *tempWord = array[i].word;
//                 array[i].word = array[j].word;
//                 array[j].word = tempWord;
//             }
//         }
//     }
//     // printf("Done sorting...\n");
//     return array;
// }

node *myMergeSort(node *a, node *tmp, int size, char *app)
{
    msort(app, a, tmp, 0, size - 1);
    return a;
}

void msort(char *app, node *a, node *tmp, int left, int right)
{
    int mid;
    if (right > left)
    {
        mid = (right + left) / 2;
        msort(app, a, tmp, left, mid);
        msort(app, a, tmp, mid + 1, right);
        merge(app, a, tmp, left, mid + 1, right);
    }
}

void merge(char *app, node *a, node *tmp, int left, int mid, int right)
{
    int i, left_end, count, tmp_pos;
    left_end = mid - 1;
    tmp_pos = left;
    count = right - left + 1;

    while ((left <= left_end) && (mid <= right))
    {
        if (strcmp(app, "-wordcount") == 0 && strcmp(a[left].word, a[mid].word) <= 0)
        {
            tmp[tmp_pos] = a[left];
            tmp_pos = tmp_pos + 1;
            left = left + 1;
        }
        else if (strcmp(app, "-sort") == 0 && atoi(a[left].word) <= atoi(a[mid].word))
        {
            tmp[tmp_pos] = a[left];
            tmp_pos = tmp_pos + 1;
            left = left + 1;
        }
        else
        {
            tmp[tmp_pos] = a[mid];
            tmp_pos = tmp_pos + 1;
            mid = mid + 1;
        }
    }
    while (left <= left_end)
    {
        tmp[tmp_pos] = a[left];
        left = left + 1;
        tmp_pos = tmp_pos + 1;
    }
    while (mid <= right)
    {
        tmp[tmp_pos] = a[mid];
        mid = mid + 1;
        tmp_pos = tmp_pos + 1;
    }
    for (i = 0; i < count; i++)
    {
        a[right] = tmp[right];
        right = right - 1;
    }
}
