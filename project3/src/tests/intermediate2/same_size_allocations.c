// REQUIRED-5: A series of same-size allocations should work correctly.

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

   // allocate three times
   void *allocated1 = kalloc_heap(ALLOCATION_SIZE, 0, heap);
   void *allocated2 = kalloc_heap(ALLOCATION_SIZE, 0, heap);
   void *allocated3 = kalloc_heap(ALLOCATION_SIZE, 0, heap);

   // the total size of one allocation
   size_t total_size = ALLOCATION_SIZE +
                     sizeof(struct header) +
                     sizeof(struct footer);

   // get the header and footer of each allocation
   struct header *head1 = allocated1 - sizeof(struct header);
   struct header *head2 = allocated2 - sizeof(struct header);
   struct header *head3 = allocated3 - sizeof(struct header);

   // the allocations should be next to each other in memory like so:
   // +-----------------------------------------------------------------------+
   // | head1 | data1 | foot1 | head2 | data2 | foot2 | head3 | data3 | foot3 |
   // +-----------------------------------------------------------------------+
   t_assert("The first and second allocations should be adjacent",
            allocated1 + total_size == allocated2);
   t_assert("The second and third allocations should be adjacent",
            allocated2 + total_size == allocated3);

   // test to make sure that all are marked as allocated
   t_assert("The header should indicate that the first block is allocated",
            head1->allocated == 1);
   t_assert("The header should indicate that the second block is allocated",
            head2->allocated == 1);
   t_assert("The header should indicate that the third block is allocated",
            head3->allocated == 1);

   // free second allocation
   kfree_heap(allocated2, heap);

   // test to make sure that the second block is not allocated
   t_assert("The header should indicate that the first block is allocated",
            head1->allocated == 1);
   t_assert("The header should indicate that the second block is not allocated",
            head2->allocated == 0);
   t_assert("The header should indicate that the third block is allocated",
            head3->allocated == 1);

   // make another allocation of the same size as block two - the same spot
   // should be retaken
   void *allocated4 = kalloc_heap(ALLOCATION_SIZE, 0, heap);
   t_assert("The fourth allocation should take the spot of the freed second"
            " allocation", allocated4 == allocated2);

   // free the remaining allocations
   kfree_heap(allocated1, heap);
   kfree_heap(allocated3, heap);
   kfree_heap(allocated4, heap);

   // free the heap space
   free(space);

   return 0;
}

