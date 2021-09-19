// Nichole Maldonado
// This is a header file for 
// a generic linked list.

#ifndef LLIST_H
#define LLIST_H

#include "bool.h"

typedef struct Node {
    void *val;
    struct Node *next;
} Node;

// Generic linked list.
typedef struct LList {
    struct Node *head;
    struct Node *tail;
} LList;

// Creates a linked list.
LList *create_llist();

// Appends new_val to the list.
bool append_ll(LList *list, void *new_val);

void print_ll(LList *list, void (*print_func)(void *));

// Deallocates the linked list.
void dealloc_ll(LList **list, void (*dealloc_func)(void **));

#endif