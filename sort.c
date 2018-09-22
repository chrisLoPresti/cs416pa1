#include "sort.h"

/*
    Bubble sort is used for sorting. 
    Sorting takes place on each node's char *word and not int count.
    strcmp is used for comparing each nodes char *word.
    To test the sorting method run testSort() function. 

*/

node *sort(node *head)
{
    printf("Performing lexicographical ordering\n");
    node *current;
    node *ptr;
    for (current = head; current != NULL; current = current->next)
    {
        for (ptr = head; ptr != NULL; ptr = ptr->next)
        {
            if (strcmp(ptr->word, current->word) > 0)
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
    printf("Finished lexicographical ordering\n");
    return head;
}
