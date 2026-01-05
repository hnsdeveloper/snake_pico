#include "inc/doublell.h"

#include <stdlib.h>
#include <string.h>

typedef struct DoubleLinkedList {
    Node* head;
    Node* tail;
} DoubleLinkedList;

typedef struct Node {
    struct Node* next;
    struct Node* prev;
    void* data;
} Node;

/// @brief Creates a node, by copying the data pointed by data.
/// @param data The data the node will contain. Must be non null.
/// @param data_size The size of the data the node will contain. Must be > 0.
/// @return A pointer to a Node on success or NULL on failure.
Node* create_node(void* data, size_t data_size);

/// @brief Destroys a node, freeing the data it holds and itself.
/// @param node The node to be destroyed.
void destroy_node(Node* node);

Node* create_node(void* data, size_t data_size) {
    if (data == NULL || data_size == 0) return NULL;

    Node* p = malloc(sizeof(Node));
    if (p) {
        p->data = NULL;
        p->next = NULL;
        p->prev = NULL;
        void* new_data = malloc(data_size);
        if (new_data == NULL) {
            free(p);
            p = NULL;
        } else {
            memcpy(new_data, data, data_size);
            p->data = new_data;
        }
    }

    return p;
}

void destroy_node(Node* node) {
    if (node != NULL) {
        free(node->data);
        free(node);
    }
}

DoubleLinkedList* create_linked_list() {
    DoubleLinkedList* p = malloc(sizeof(DoubleLinkedList));
    if (p != NULL) {
        p->head = NULL;
        p->tail = NULL;
    }

    return p;
}

void destroy_linked_list(DoubleLinkedList* list) {
    if (list != NULL) {
        Node* n = list->head;
        while (n != NULL) {
            Node* prev = n;
            n = prev->next;
            destroy_node(prev);
        }
        free(list);
    }
}

bool prepend_data(DoubleLinkedList* list, void* data, size_t data_size) {
    Node* n = create_node(data, data_size);
    if (n == NULL) return false;

    n->next = list->head;
    if (list->head != NULL)
        list->head->prev = n;
    else {
        list->tail = n;
    }

    list->head = n;

    return true;
}

void in_place_map_from_tail(DoubleLinkedList* list, in_place_map_function fn,
                            void* extra_data) {
    if (list != NULL) {
        Node* n = list->tail;
        while (n) {
            fn(n->data, extra_data);
            n = n->prev;
        }
    }
}

void reduce_from_tail(DoubleLinkedList* list, reduce_function fn,
                      void* accumulator, void* extra_data) {
    if (list != NULL && accumulator != NULL) {
        Node* n = list->tail;
        while (n) {
            fn(n->data, accumulator, extra_data);
            n = n->prev;
        }
    }
}

void* get_head(DoubleLinkedList* list) {
    if (list != NULL && list->head != NULL) return list->head->data;
    return NULL;
}

void* get_tail(DoubleLinkedList* list) {
    if (list != NULL && list->tail != NULL) return list->tail->data;
    return NULL;
}

void for_each(DoubleLinkedList* list, for_each_function fn, void* extra_data) {
    if (list != NULL) {
        Node* n = list->head;
        while (n != NULL) {
            fn(n->data, extra_data);
            n = n->next;
        }
    }
}