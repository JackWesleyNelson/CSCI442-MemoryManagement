// A sorted array that holds void* items - implementation

#include "sorted_array.h"

#include "memset.h"

struct sorted_array sorted_array_place(void *addr,
                                       size_t max_size,
                                       comparison_predicate_t comparison)
{
   // the array to return
   struct sorted_array array;

   // set the pointer to the memory to use and clear the memory
   array.storage = addr;
   memset(array.storage, 0, max_size * sizeof(void *));

   // set the size, max size, and comparison function
   array.size = 0;
   array.max_size = max_size;
   array.comparison = comparison;

   return array;
}

void sorted_array_insert(void *item, struct sorted_array *array)
{
   // iterator counter
   size_t i = 0;

   // FIXME: max size check before doing anything

   // traverse while the current item is less than the item being inserted, or
   // the end is reached
   while(i < array->size && array->comparison(array->storage[i], item) < 0) {
       i++;
   }

   // ended iteration - at the end of the sorted array
   if(i == array->size)
   {
      // at the end of the array - add at the end of the array
      array->storage[array->size++] = item;
   }
   else
   {
      // somewhere in the middle
      // move all elements after this element

      // the current element in this slot
      void *tmp = array->storage[i];

      // put the item being inserted into the slot
      array->storage[i] = item;

      // shift everything after the current elemtn down by one
      while(i < array->size)
      {
         i++;
         void *tmp2 = array->storage[i];
         array->storage[i] = tmp;
         tmp = tmp2;
      }

      array->size++;
   }
}

void *sorted_array_lookup(size_t i, struct sorted_array *array)
{
   if(i >= array->size) {
      return NULL;
   }

   return array->storage[i];
}

void sorted_array_remove(size_t i, struct sorted_array *array)
{
   if(i >= array->size) {
      return;
   }

   while (i < array->size)
   {
       array->storage[i] = array->storage[i+1];
       i++;
   }
   array->size--;
}
