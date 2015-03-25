// REQUIRED-10: Basic kalloc/kfree calls work.

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

   void *allocated = kalloc_heap(20, 0, heap);

   // assert that the allocation is in the correct region
   t_assert("The returned pointer should be within the allocated space",
            allocated > space);
   t_assert("The returned pointer should be within the allocated space",
            allocated < space + SPACE_SIZE_TOTAL);

   // free the allocation
   kfree_heap(allocated, heap);

   // free the heap space
   free(space);

   return 0;
}
