// REQUIRED-30: page alignment: basic allocations work

#include <stdlib.h>

#include "../test.h"
#include "../../kheap.h"

#define SPACE_SIZE_INITIAL  (5 * 1024 * 1024)   // 5MiB
#define SPACE_SIZE_TOTAL    (10 * 1024 * 1024)  // 10MiB

#define ALLOCATION_SIZE     20

// defined in kheap.c
void *align(void *p);

int main(int argc, char **argv)
{
   // allocate space for kmalloc to play with
   void *space = malloc(SPACE_SIZE_TOTAL);

   // create the heap
   struct heap *heap = heap_create(space,
                                   space + SPACE_SIZE_INITIAL,
                                   space + SPACE_SIZE_TOTAL);

   // aligned allocation
   void *allocated = kalloc_heap(ALLOCATION_SIZE, 1, heap);

   void *allocated_aligned = align(allocated);

   t_assert("The aligned allocation should be aligned properly",
            allocated == allocated_aligned);
   t_assert("The aligned allocation should not be null",
            allocated != NULL);

   // free the allocations
   kfree_heap(allocated, heap);

   // free the heap space
   free(space);

   return 0;
}

