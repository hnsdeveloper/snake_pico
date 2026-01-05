#ifndef _DOUBLE_LL_H_
#define _DOUBLE_LL_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct Node Node;
typedef struct DoubleLinkedList DoubleLinkedList;

/// @brief Creates a new double linked list.
/// @return A pointer to an empty double linked list on success or NULL on
/// failure.
DoubleLinkedList* create_linked_list();

/// @brief Destroys a double linked list, freeing all the nodes and freeing the
/// data those nodes hold.
/// @param list The list to be destroyed.
void destroy_linked_list(DoubleLinkedList* list);

/// @brief Prepends some data to the linked list by copying it.
/// @param list The linked list to which we want to prepend the data.
/// @param data The data which we want to prepend.
/// @param data_size The size of the data we want to prepend.
/// @return True on success, false on failure.
bool prepend_data(DoubleLinkedList* list, void* data, size_t data_size);

/// @brief Function for in place mapping of elements in a double linked list.
/// The first argument is the data in the data strucuture and the second is an
/// optional data argument.
typedef void (*in_place_map_function)(void*, void*);

/// @brief In place map from tail function. Will apply a function for each
/// element of the linked list from the tail.
/// @param list The list itself.
/// @param fn The mapping function.
/// @param extra_data Extra data for the mapping function.
void in_place_map_from_tail(DoubleLinkedList* list, in_place_map_function fn,
                            void* extra_data);

/// @brief Function for reducing. The first argument is the data inside the node
/// of the linked list. The second is the accumulator. The third is for extra
/// data.
typedef void (*reduce_function)(const void*, void*, void*);

/// @brief Reduce function. Will do it from the tail. Will return immediately if
/// any argument except extra_data is null.
/// @param list The list itself.
/// @param fn The reducing function.
/// @param accumulator A pointer to the accumulator.
/// @param extra_data An optional pointer to extra data.
void reduce_from_tail(DoubleLinkedList* list, reduce_function fn,
                      void* accumulator, void* extra_data);

typedef void (*for_each_function)(const void*, void*);

/// @brief Iterates over all elements of the linked list, calling the function
/// fn with the node data and extra_data as arguments.
/// @param list The list itself.
/// @param fn The function to be called.
/// @param extra_data Optional extra data.
void for_each(DoubleLinkedList* list, for_each_function fn, void* extra_data);

/// @brief Gets the data held by the head node.
/// @param list The list itself.
/// @return A pointer to the data or NULL if the list is empty.
void* get_head(DoubleLinkedList* list);

/// @brief Gets the data held by the tail node.
/// @param list The list itself.
/// @return A pointer to the data or NULL if the list is empty.
void* get_tail(DoubleLinkedList* list);

#endif