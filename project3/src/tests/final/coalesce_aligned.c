// REQUIRED-5: page alignment: coalesce with hole in front

#include <stdlib.h>

#include "../test.h"
#include "../../kheap.h"

#define SPACE_SIZE_INITIAL  (5 * 1024 * 1024)   // 5MiB
#define SPACE_SIZE_TOTAL    (10 * 1024 * 1024)  // 10MiB

int main(int argc, char **argv)
{
   // allocate space for kmalloc to play with
   void *space = malloc(SPACE_SIZE_TOTAL);

   // create the heap
   struct heap *heap = heap_create(space,
                                   space + SPACE_SIZE_INITIAL,
                                   space + SPACE_SIZE_TOTAL);

   // page-aligned allocation
   void *allocated = kalloc_heap(20, 1, heap);
   struct header *head = allocated - sizeof(struct header);
   t_assert("The returned pointer should not be NULL",
            allocated != NULL);

   t_assert("The size of the free list should be 2",
            heap->free_list.size == 2);

   // the free
   kfree_heap(allocated, heap);

   // with coalescing, there should only be one chunk
   t_assert("The size of the free list should be 1 after coalescing",
            heap->free_list.size == 1);

   // free the heap space
   free(space);

   return 0;
}
