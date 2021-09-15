#ifndef LLIST_H
#define LLIST_H

#include "bool.h"

typedef struct Node {
    void *val;
    struct Node *next;
} Node;

typedef struct LList {
    struct Node *head;
    struct Node *tail;
} LList;

//bool append(LList *list, void *new_val, size_t val_size);
bool append(LList *list, void *new_val);

bool is_empty(LList *list);

void print_ll(LList *list, void (*print_func)(void *));

void dealloc_node(Node *node, void (*dealloc_func)(void **));

void dealloc_ll(LList **list, void (*dealloc_func)(void **));

char **convert_llist_to_char_list(LList **list);

int size_ll(LList *list);

void append_ll(LList *main_list, LList **append_list);

#endif