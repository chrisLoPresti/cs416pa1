#ifndef struct_h
#define struct_h

struct node
{
    char *word;
    struct node *next;

} typedef node;

struct treeNode
{
    char *word;
    int count;
    struct treeNode *left;
    struct treeNode *right;

} typedef treeNode;

#endif