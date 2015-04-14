// REQUIRED-20: coalescing (double): works

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

   // first allocation
   void *allocated1 = kalloc_heap(20, 0, heap);
   struct header *head1 = allocated1 - sizeof(struct header);
   t_assert("The returned pointer should not be NULL",
            allocated1 != NULL);
   t_assert("The size of the free list should be 1",
            heap->free_list.size == 1);

   // second allocation
   void *allocated2 = kalloc_heap(20, 0, heap);
   struct header *head2 = allocated2 - sizeof(struct header);
   t_assert("The returned pointer should not be NULL",
            allocated2 != NULL);
   t_assert("The size of the free list should be 1",
            heap->free_list.size == 1);

   // free the first chunk
   kfree_heap(allocated1, heap);

   // with coalescing, there should only be two chunks (one at start and one
   // at end)
   t_assert("The size of the free list should be 2 after the first free",
            heap->free_list.size == 2);

   // free the second chunk
   kfree_heap(allocated2, heap);

   // with coalescing, there should only be one chunk after both frees
   t_assert("The size of the free list should be 1 after coalescing",
            heap->free_list.size == 1);

   // free the heap space
   free(space);

   return 0;
}
