// A sorted array that holds void* items

#ifndef SORTED_ARRAY_H
#define SORTED_ARRAY_H

#include "common.h"

// a sorted array that stores anything that can be cast to a void*

// a function that returns -1 if a < b, 0 if a == b, 1 if a > b, where a is the
// first argument, and b is the second argument
typedef s8int (*comparison_predicate_t)(void *, void *);

// the main sorted array struct
struct sorted_array
{
   void **storage;
   size_t size;
   size_t max_size;
   comparison_predicate_t comparison;
};

// creates a sorted array of a maximum size with a particular comparison
// function - kmalloc must be working
struct sorted_array sorted_array_create(size_t max_size,
                                        comparison_predicate_t comparison);

// places a sorted array of a maximum size with a particular comparison
// function at the specified memory address
struct sorted_array sorted_array_place(void *addr,
                                       size_t max_size,
                                       comparison_predicate_t comparison);
// removes a sorted array
void sorted_array_destroy(struct sorted_array *array);

// adds an item to the array
void sorted_array_insert(void *item, struct sorted_array *array);

// returns the item at index i
// if the index is invalid, NULL is returned
void *sorted_array_lookup(size_t i, struct sorted_array *array);

// removes the item at index i from the array
void sorted_array_remove(size_t i, struct sorted_array *array);

#endif // SORTED_ARRAY_H
