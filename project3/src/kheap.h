// Heap memory allocator

#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"
#include "sorted_array.h"

#define HEAP_MAGIC          0x23456789
#define HEAP_FREE_LIST_SIZE 0x20000

// header information for a memory block/hole
struct header
{
   u32int magic;     // magic number (used for identification / error checking)
   size_t size;      // size of the block (including both the header and footer)
   u8int  allocated; // 1 if in use; 0 if free
};

// footer information for a memory block/hole
struct footer
{
   u32int magic;          // magic number, same as in header_t.
   struct header *header; // pointer to the block header
};

// the basic structure of the heap
struct heap
{
   struct sorted_array free_list;
   void   *start_address; // the start of the space in which memory can be
                          // allocated (free_list is not included)
   void   *end_address;   // the end of the allocated space
   void   *max_address;   // the max address to which the heap can be expanded
                          // past the end of end_address
};

// creates a heap
// start is the start point
// end is the end of the allocated region
// max is the maximum point to which the heap can expand
struct heap *heap_create(void *start, void *end, void *max);

// allocates a continguous region of memory that is of size 'size'
// if page_align is 1, then the returned memory is aligned on a page boundary
void *kalloc_heap(size_t size, u8int page_align, struct heap *heap);

// releases a block that was allocated using kalloc
// p is the pointer to release
// heap is the heap that the memory came from
void kfree_heap(void *p, struct heap *heap);

#endif // KHEAP_H
