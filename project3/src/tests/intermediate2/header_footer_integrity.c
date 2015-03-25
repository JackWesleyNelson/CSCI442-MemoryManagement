// REQUIRED-5: The header and footer are set correctly for the returned region.

#include <stdlib.h>

#include "../test.h"
#include "../../kheap.h"

#define SPACE_SIZE_INITIAL  (5 * 1024 * 1024)   // 5MiB
#define SPACE_SIZE_TOTAL    (10 * 1024 * 1024)  // 10MiB

#define ALLOCATION_SIZE     20

int main(int argc, char **argv)
{
   // allocate space for kmalloc to play with
   void *space = malloc(SPACE_SIZE_TOTAL);

   // create the heap
   struct heap *heap = heap_create(space,
                                   space + SPACE_SIZE_INITIAL,
                                   space + SPACE_SIZE_TOTAL);

   void *allocated = kalloc_heap(ALLOCATION_SIZE, 0, heap);

   // get the header and footer
   struct header *head = allocated - sizeof(struct header);
   struct footer *foot = allocated + ALLOCATION_SIZE;

   t_assert("The header magic number should be correct",
            head->magic == HEAP_MAGIC);
   t_assert("The footer magic number should be correct",
            foot->magic == HEAP_MAGIC);
   t_assert("The header should indicate that the block is allocated",
            head->allocated == 1);
   t_assert("The footer head pointer should point to the header",
            foot->header == head);
   t_assert("The size of the allocation should be correct",
            head->size == ALLOCATION_SIZE +
                          sizeof(struct header) +
                          sizeof(struct footer));

   // free the allocation
   kfree_heap(allocated, heap);

   // the header should not indicate that the block is free
   // since there was only one allocation, there should be no coalescing
   t_assert("The header should indicate that the block is not allocated",
            head->allocated == 0);

   // free the heap space
   free(space);

   return 0;
}
