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

node *sortProcs(node *array, int length, char *app)
{
    int numbers = strcmp(app, "-sort") == 0 ? 1 : 0;
    int i, j;
    for (i = 0; i < length; ++i)
    {
        for (j = i; j < length; ++j)
        {
            if (!numbers && strcmp(array[j].word, array[i].word) < 0)
            {
                char *tempWord = array[i].word;
                array[i].word = array[j].word;
                array[j].word = tempWord;
            }
            else if (atoi(array[j].word) < atoi(array[i].word))
            {
                char *tempWord = array[i].word;
                array[i].word = array[j].word;
                array[j].word = tempWord;
            }
        }
    }

    return array;
}

void testSort()
{
    node *nodes;
    node *newNode1 = (node *)malloc(sizeof(node));
    node *newNode2 = (node *)malloc(sizeof(node));
    node *newNode3 = (node *)malloc(sizeof(node));
    node *newNode4 = (node *)malloc(sizeof(node));
    node *newNode5 = (node *)malloc(sizeof(node));
    newNode1->word = "a";
    newNode1->count = 1;

    newNode2->word = "b";
    newNode2->count = 2;
    newNode3->word = "f";
    newNode3->count = 3;
    newNode4->word = "d";
    newNode4->count = 4;
    newNode5->word = "e";
    newNode5->count = 5;
    newNode1->next = newNode2;
    newNode2->next = newNode3;
    newNode3->next = newNode4;
    newNode4->next = newNode5;
    newNode5->next = NULL;
    nodes = newNode1;
    node *temp = nodes;
    printf("printing unsorted link list: \n");
    while (temp != NULL)
    {
        printf("%s,%d\n", temp->word, temp->count);
        temp = temp->next;
    }
    node *head = sort(nodes, "-sort");
    printf("Printing sorted link list: \n");
    while (head != NULL)
    {
        printf("%s,%d\n", head->word, head->count);
        head = head->next;
    }
}