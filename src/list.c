#include "list.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "util.h"

/**
 * Struct for encapsulating a single list element.
 */
typedef struct ListItem {
    struct ListItem* next;  // pointer to the next element (NULL if last)
    void* data;             // pointer to the data
} ListItem;

List mkList(void) {
    List res;
    res.head = NULL;
    return res;
}

void clearList(List* s) {
    ListItem* current = s->head;
    while (current != NULL) {
        ListItem* next = current->next;
        free(current);
        current = next;
    }
    s->head = NULL;
}

void push(List* s, void* data) {
    if (s == NULL) {
        return;  // Or handle the error in an appropriate way
    }

    ListItem* newItem = (ListItem*)malloc(sizeof(ListItem));
    // ListItem* newItem;
    if (newItem == NULL) {
        // Error: Failed to allocate memory for the new item
        exit(EXIT_FAILURE);  // Or handle the error in an appropriate way
    }

    newItem->data = data;
    newItem->next = s->head;
    s->head = newItem;
}

void* peek(List* s) {
    if (s->head == NULL) {
        // List is empty, nothing to peek
        err("List is empty nothing to peek");
    }

    return s->head->data;
}

void pop(List* s) {
    if (s->head == NULL) {
        // List is empty, nothing to pop
        return;
    }

    ListItem* toRemove = s->head;
    s->head = s->head->next;
    free(toRemove);
}

char isEmpty(List* s) { return (s->head == NULL); }

ListIterator mkIterator(List* list) {
    ListIterator res;
    res.list = list;
    res.prev = NULL;
    res.current = list->head;

    return res;
}

void* getCurr(ListIterator* it) {
    assert(it->current != NULL);
    return it->current->data;
}

void next(ListIterator* it) {
    assert(isValid(it));
    it->prev = it->current;
    it->current = it->current->next;
}

char isValid(ListIterator* it) { return it->current != NULL; }
