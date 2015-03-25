// REQUIRED-5: A series of different-size allocations should work correctly.

#include <stdlib.h>

#include "../test.h"
#include "../../kheap.h"

#define SPACE_SIZE_INITIAL  (5 * 1024 * 1024)   // 5MiB
#define SPACE_SIZE_TOTAL    (10 * 1024 * 1024)  // 10MiB

#define ALLOCATION_SIZE_S   20
#define ALLOCATION_SIZE_L   200

int main(int argc, char **argv)
{
   // allocate space for kmalloc to play with
   void *space = malloc(SPACE_SIZE_TOTAL);

   // create the heap
   struct heap *heap = heap_create(space,
                                   space + SPACE_SIZE_INITIAL,
                                   space + SPACE_SIZE_TOTAL);

   // allocate three times
   void *allocated1 = kalloc_heap(ALLOCATION_SIZE_L, 0, heap);
   void *allocated2 = kalloc_heap(ALLOCATION_SIZE_L, 0, heap);
   void *allocated3 = kalloc_heap(ALLOCATION_SIZE_L, 0, heap);

   // the total size of one allocation
   size_t total_size_small = ALLOCATION_SIZE_S +
                             sizeof(struct header) +
                             sizeof(struct footer);
   size_t total_size_large = ALLOCATION_SIZE_L +
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
            allocated1 + total_size_large == allocated2);
   t_assert("The second and third allocations should be adjacent",
            allocated2 + total_size_large == allocated3);

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

   // make another allocation, this time of a smaller size, fills the
   // beginning of the hole left by freeing block 2
   // in addition, a hole should be added to the right of this smaller fourth
   // block, so that memory is like so:
   // allocation1 - allocation4 - free space - allocation 3
   void *allocated4 = kalloc_heap(ALLOCATION_SIZE_S, 0, heap);
   t_assert("The fourth allocation should take the spot of the freed second"
            " allocation", allocated4 == allocated2);

   // test to make sure that there is a hole to the right of the smaller
   // allocation
   struct header *hole = allocated4 + ALLOCATION_SIZE_S + sizeof(struct footer);
   struct footer *hole_foot = allocated3 -
                              sizeof(struct header) -
                              sizeof(struct footer);
   t_assert("There should be a hole to the right of the fourth allocation",
            hole->magic == HEAP_MAGIC);
   t_assert("There should be a hole to the right of the fourth allocation",
            hole_foot->magic == HEAP_MAGIC);
   t_assert("The header pointer should be correct",
            hole_foot->header == hole);
   t_assert("The hole should be of the correct size",
            hole->size == total_size_large - total_size_small);

   // free the remaining allocations
   kfree_heap(allocated1, heap);
   kfree_heap(allocated3, heap);
   kfree_heap(allocated4, heap);

   // free the heap space
   free(space);

   return 0;
}
