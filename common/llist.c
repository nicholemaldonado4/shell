#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "llist.h"

static void print_node(Node *node, void (*print_func)(void *));

// USED and VERIFIED.
// Adds new_val to the tail end of the list. Returns
// FALSE if unsuccessful.
bool append(LList *list, void *new_val) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        return FALSE;
    }
    
    new_node->val = new_val;
    new_node->next = NULL;

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
    return TRUE;
}

// USED and VERIFIED
bool is_empty(LList *list) {
    return list->head == NULL;
}

static void print_node(Node *node, void (*print_func)(void *)) {
    if (node == NULL) {
        return;
    }
    if (node->val != NULL) {
        (*print_func)(node->val);
    }
    
    
    print_node(node->next, print_func);
}

void print_ll(LList *list, void (*print_func)(void *)) {
    print_node(list->head, print_func);
}

// USED and VERIFIED.
void dealloc_node(Node *node, void (*dealloc_func)(void **)) {
    if (node == NULL) {
        return;
    }
    if (node->val != NULL) {
        dealloc_func(&(node->val));
    }
    Node *next_node = node->next;
    free(node);
    node = NULL;
    dealloc_node(next_node, dealloc_func);
}

// USED and VERIFIED.
void dealloc_ll(LList **list, void (*dealloc_func)(void **)) {
    if (list == NULL || *list == NULL) {
        return;
    }
    Node *curr = (*list)->head;
    while (curr != NULL) {
        Node *next_node = curr->next;
        dealloc_func(&(curr->val));
        free(curr);
        curr = next_node;
    }
    free(*list);
    *list = NULL;
}

// USED and VERIFIED.
static char **convert_to_char_list(Node *node, int num_nodes) {
    if (node == NULL) {
        char **char_list = (char **)malloc((num_nodes + 1) * sizeof(char *));
        if (char_list == NULL) {
            return char_list;
        }
        char_list[num_nodes] = NULL;
        return char_list;
    }
    char **char_list = convert_to_char_list(node->next, num_nodes + 1);
    if (char_list == NULL) {
        return char_list;
    }
    char_list[num_nodes] = (char *)node->val;
    return char_list;
}

char **convert_llist_to_char_list(LList **list) {
    char **char_list = convert_to_char_list((*list)->head, 0);
    int i = 0;
    return char_list;
}

int size_ll(LList *list) {
    int size = 0;
    Node *curr = list->head;
    while (curr != NULL) {
        size++;
        curr = curr->next;
    }
    return size;
}

// Appends append-list to main_list and deallocates append_list;
void append_ll(LList *main_list, LList **append_list) {
    if (is_empty(main_list)) {
        main_list->head = (*append_list)->head;
        main_list->tail = (*append_list)->tail;
        
    } else {
        main_list->tail->next = (*append_list)->head;
        main_list->tail = (*append_list)->tail;
    }
    
    (*append_list)->head = NULL;
    (*append_list)->tail = NULL;
    free(*append_list);
    *append_list = NULL;
}