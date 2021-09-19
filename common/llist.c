// Nichole Maldonado
// This is a file for a generic 
// linked list.

#include "llist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"

static void print_node(Node *node, void (*print_func)(void *));

/*
 * Creates a linked list.
 * Input: None.
 * Output: A generic linked list.
 */
LList *create_llist() {
    LList *llist = (LList *)malloc(sizeof(LList));
    if (llist == NULL) {
        return NULL;
    }
    llist->head = NULL;
    llist->tail = NULL;
    return llist;
}

/*
 * Appends a node with new_val to the list.
 * Input: The list and value to be added to it.
 * Output: True if append was successful, false otherwise.
 */
bool append_ll(LList *list, void *new_val) {
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

/*
 * Deallocates the linked list.
 * Input: The linked list and deallocator function
 *        for the node's value.
 * Output: False.
 */
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