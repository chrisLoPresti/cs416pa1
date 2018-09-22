#ifndef struct_h
#define struct_h

struct node
{
    char *word;
    int count;
    struct node *next;

} typedef node;

struct inputData
{
    char *word;
    struct inputData *next;

} typedef inputData;

struct bucket
{
    int id;
    struct inputData *keys;
    struct bucket *next;
} typedef bucket;

#endif