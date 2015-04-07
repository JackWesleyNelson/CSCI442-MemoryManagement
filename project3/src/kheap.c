// Implementation of the heap

#include "kheap.h"

#include "common.h"

// headers for local functions
void *align(void *p);
ssize_t find_smallest_hole(size_t size,
                           u8int page_align,
                           struct heap *heap)
                           WARN_UNUSED;
s8int header_less_than(void *a, void *b);
s8int heap_resize(size_t new_size, struct heap *heap) WARN_UNUSED;
void add_hole(void *start, void *end, struct heap *heap);

// returns an aligned pointer
// if the address is not aligned, then the aligned address prior to the given
// address is returned
void *align(void *p)
{
   union
   {
      void *pointer;
      size_t integer;
   } u;

   u.pointer = p;
   u.integer &= PAGE_MASK;

   return u.pointer;
}

// less than function for comparing the sizes of memory chunks, using the
// headers; a < b iff a's size is less than b's size
// a and b should both be pointers to header structs
s8int header_less_than(void *a, void *b)
{
   return (((struct header*)a)->size < ((struct header*)b)->size) ? 1 : 0;
}

// creates a heap at the given start address, end address, and maximum growth
// size
// start, end, and max should all be page-aligned (but if they aren't, we just
// waste some space)
struct heap *heap_create(void *start,
                         void *end,
                         void *max)
{
   // in the real kernel, we would kmalloc here, where kmalloc is just a
   // placement implementation, since the heap does not exist yet!
   // for the userspace projet, we can assume that the memory has already been
   // allocated
   //
   // the memory layout from start to end is as follows:
   // | heap struct | free list | actual data |
   //struct heap *heap = (struct heap*)kmalloc(sizeof(heap_t));
   struct heap *heap = (struct heap*)start;

   // create the free list
   heap->free_list = sorted_array_place((void *)start +
                                           sizeof(struct sorted_array),
                                     HEAP_FREE_LIST_SIZE,
                                     &header_less_than);

   // move the start address of the heap, to reflect where data can be place,
   // now that the free list is in the initial portion of the heap's memory
   // address space
   start += sizeof(struct sorted_array) + sizeof(void *) * HEAP_FREE_LIST_SIZE;

   // make sure the start address is page-aligned
   if(align(start) != start) {
      start = align(start) + PAGE_SIZE;
   }

   // write the avariables into the heap structure
   heap->start_address = start;
   heap->end_address = end;
   heap->max_address = max;

   // start with a large hole the size of the memory region
   add_hole(start, end, heap);

   return heap;
}

// expands or contracts the heap to the new_size
// returns a negative value on error, 0 on success
s8int heap_resize(size_t new_size, struct heap *heap)
{
   // make sure the new end address is page-aligned
   // since the heap starts on a page boundary, we need only align the size
   if(align((void *)new_size) != (void *)new_size) {
      new_size = (size_t)(align((void *)new_size)) + PAGE_SIZE;
   }

   if(new_size < heap->end_address - heap->start_address)
   {
      // contracting the heap

      // we are going to naively assume that the heap is not being resized to
      // a value that is too small

      // paging code would go here to free pages
      // for now, just assume that in our flat memory space, memory is
      // available and does not need to be allocated or freed
   }
   else if(new_size > heap->end_address - heap->start_address)
   {
      // expanding the heap

      // make sure the new size is within the bounds
      if(heap->start_address + new_size > heap->max_address) {
         // not within bounds
         return -1;
      }

      // paging code would be here to allocate pages
      // for now, just assume that in our flat memory space, memory is
      // available and does not need to be allocated or freed
   }
   else
   {
      // same size - do nothing
   }

   // set the size of the heap
   heap->end_address = heap->start_address + new_size;

   return 0;
}

// find the smallest hole that will fit the requested size
// if a hole is found, the index in the heap free list is returned
// if a hole is not found, then -1 is returned
// size must include the size of the header and footer, in addition to the
// size that the actual users wishes to request
ssize_t find_smallest_hole(size_t size,
                           u8int page_align,
                           struct heap *heap)
{
   // TODO: IMPLEMENT THIS FUNCTION

   // pseudocode:
   // 1: iterate over free list
   // 2: see if the chunk is large enough for the requested size - remember to
   //    include page alignment!
   // 3: if large enough, return
   // 4: if not large enough, continue to the next chunk in the iteration
   // 5: if the end is reached before a chunk is found, return -1

   return 0;
}

// creates and writes a hole that spans [start,end)
void add_hole(void *start, void *end, struct heap *heap)
{
   //make header
   struct header h;
   h.magic = HEAP_MAGIC;
   h.size = end - start; //NOTE: DO WE NEED ERROR CHECKING HERE? if (end < start)?
   h.allocated = 0;  // 0 = unallocated

   //make footer
   struct footer f;
   f.magic = HEAP_MAGIC;
   f.header = &h;

   //write header and footer to memory
   sorted_array_insert(&h, &(heap->free_list));


   // pseudocode:
   // 0. determine if coalesing is possible; if so, remove the appropriate
   //    holes on either side of start and end, and then call add_hole
   //    recursively on the larger region (can skip if no coalesing)
   // 1. write header and footer to memory
   // 2. add chunk to free list
}

void *kalloc_heap(size_t size, u8int page_align, struct heap *heap)
{
   // TODO: IMPLEMENT THIS FUNCTION

   // pseudocode:
   // 1. figure out size of needed free list entry (add size of header and
   //    footer with sizeof(struct header) / sizeof(struct footer))
   // 2. find a hole to allocate using find_smallest_hole
   // 3. if no hole found, resize the heap, and then start again at 2
   // 4. remove the found hole from the free list to use for allocation
   // 5. page-align, if necessary
   // 5.1. determine if page-alignment makes a good hole before our
   //      allocation; if so, add that hole
   // 6. mark the chunk as allocated, write the header/footer (if necessary)
   // 7. return pointer to allocated portion of memory

   return NULL;
}

void kfree_heap(void *p, struct heap *heap)
{
   // TODO: IMPLEMENT THIS FUNCTION

   // pseudocode:
   // 1. check for a null pointer before proceeding
   // 2. get the header and the footer based on the passed pointer
   // 3. mark the chunk as free in the header
   // 4. add a hole in the space that was previously allocated
}
