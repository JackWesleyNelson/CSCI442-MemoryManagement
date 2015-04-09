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
      if (start & 0xFFFFF000 != 0){
         start &= 0xFFFFF000;
         start += 0x1000;
      }

      // we are going to naively assume that the heap is not being resized to
      // a value that is too small

      // paging code would go here to free pages
      // for now, just assume that in our flat memory space, memory is
      // available and does not need to be allocated or freed
   }
   else if(new_size > heap->end_address - heap->start_address)
   {
      // expanding the heap
      if (new_size & 0xFFFFF000 != 0){
         new_size &= 0xFFFFF000;
         new_size += 0x1000;
      }

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

   // pseudocode:
   // 1: iterate over free list
   // 2: see if the chunk is large enough for the requested size - remember to
   //    include page alignment!
   // 3: if large enough, return
   // 4: if not large enough, continue to the next chunk in the iteration
   // 5: if the end is reached before a chunk is found, return -1

	//create size_t variable to use for iteration
   size_t i = 0;

   //header *header = (header *)sorted_array_lookup(i, &heap->)

	//iterate over size of free_list until a chunk is found
	for(i; i < heap->free_list.size; i ++){

      struct header *header = (struct header *)sorted_array_lookup(i , &heap->free_list);

      size_t loc = (size_t)header;
      size_t offset = 0;

      //calulate size of hole after you take out the header, this will give the size of what is actually
      //available in each chunk, once page alignment has been taken into account.

      if(page_align)
      {
         offset = PAGE_SIZE - (loc + sizeof(struct header))%PAGE_SIZE;
      }
      size_t hole_size = header->size - offset;

      if(hole_size >= size)
      {
         return i;
      }

		//is the size contained in header larger than size needed?
		//if(size < heap->free_list.size){
			//return the chunk of memory;
		//	return heap->free_list.storage[i];
		//}
	}
	//no chunk with a valid size is found, return -1;
  return -1;
}

// creates and writes a hole that spans [start,end)
void add_hole(void *start, void *end, struct heap *heap)
{
   //make header
   struct header *h;
   h = (struct header *) start;
   h->magic = HEAP_MAGIC;
   h->size = end - start; //NOTE: DO WE NEED ERROR CHECKING HERE? if (end < start)?
   h->allocated = 0;  // 0 = unallocated

   //make footer
   struct footer *f;
   f = (struct footer *) end - sizeof(f);
   f->magic = HEAP_MAGIC;
   f->header = h;

   
   // add chunk to free list
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

   size_t new_size = size + sizeof(struct header) + (sizeof(struct footer));

   //use our find smallest hole
   size_t iterator_result = find_smallest_hole(new_size, page_align, heap);

   //if iterator is -1, then we didnt find a hole, otherwise allocate chuck at iterator location
   if(iterator_result == -1)
   {
      //error check/reporting
      //save the heap length and end address
      size_t old_length = heap->end_address - heap->start_address;
      size_t old_end_address = heap->end_address;
      //allocate more room
      size_t resize_result = heap_resize(old_length + new_size, heap);
      //if(resize_result == -1){
      	//heap did not resize, error out here
      	//not sure how since we don't get to use the library for fprintf and stderr
      //}
      //heap did resize
      size_t new_length = heap->end_address - heap->start_address;
      //find location of the last header in memory
      iterator_result = 0;
      //index and value of furthest header
      size_t index = -1;
      size_t value = 0x0;
      while(iterator_result < heap->free_list.size)
      {
      	//get value of the header
      	size_t tmp = (size_t)sorted_array_lookup(iterator_result, &heap->free_list);
      	//replace value with this header value if it's larger, and set the index to the current iteration
      	if(tmp > value)
      	{
      		value = tmp;
      		index = iterator_result;
      	}
      }
      
      //if no headers were found, add a header.
      if(index == -1){
      	//**********This block might be replaced by just the add hole function? ***********
      	
      	//set the pointer of a new header to the end of the old heap
      	struct header *new_header = (struct header *)old_end_address;
      	//set the magic number, length, and 0 for is allocated
      	new_header->magic = HEAP_MAGIC;
      	new_header->size = new_length - old_length;
      	new_header->allocated = 0;
      	//set the pointer of a new footer to the end of the space allocated, - the footer size
      	struct footer *new_footer = (struct footer *)(old_end_address + new_header->size - sizeof(struct footer));
      	//set the magic number, and the associated header
      	new_footer->magic = HEAP_MAGIC;
      	new_footer->header = new_header;
      	//insert the header into the free_list
      	sorted_array_insert((struct header *)new_header, &heap->free_list);
      }
      else
      {
      	//a last header was found, adjust it
      	struct header *new_header = sorted_array_lookup(index, &heap->free_list);
      	//add the difference in sizes to be the new size
      	new_header->size += new_length - old_length;
      	struct footer *new_footer = (struct footer *)((size_t)new_header + new_header->size - sizeof(struct footer));
      	new_footer->magic = HEAP_MAGIC;
      	new_footer->header = new_header;
      }
      //Now that we should have enough space, recall the function
      return kalloc_heap(size, page_align, heap);
   }

   struct header *old_hole_header = (struct header *)sorted_array_lookup(iterator_result, &heap->free_list);
   size_t old_hole_loc = (size_t)old_hole_header;
   size_t old_hole_size = old_hole_header->size;


   if((old_hole_size - new_size) < (sizeof(struct header) + sizeof(struct footer)))
   {
      size = size + old_hole_size - new_size;
      new_size = old_hole_size;
   }

   //page alignment //TODO: make sure this is all correct
   if(page_align && old_hole_loc&0xFFFFF000){
      size_t new_loc = old_hole_loc + PAGE_SIZE - (old_hole_loc&0xFFF) - sizeof(struct header);
      struct header *hole_header = (struct header *)old_hole_loc;
      hole_header->size = PAGE_SIZE - (old_hole_loc&0xFFF) - sizeof(struct header);
      hole_header->magic = HEAP_MAGIC;
      //set chunk as allocated
      hole_header->allocated = 1;

      struct footer *hole_foot = (struct footer *)((size_t)new_loc - sizeof(struct footer));
      hole_foot->magic = HEAP_MAGIC;
      hole_foot->header = hole_header;
      old_hole_loc = new_loc;
      old_hole_size = old_hole_size - hole_header->size;

      //TODO
      // Can you guys figure out what (old_hole_loc&0xFFF) is? I don't quite understand it. It may be replaceable by PAGE_MASK
      //but im not sure
      //Answer: old_hole_loc is the memory address of the location, 
      //old_hole_loc&0xFFF maintains the highest order 12 bits of old_hole_loc
      //while setting the remaining bits to 0
   }


   //create replacement headers for the chunk
   struct header *chunk_header = (struct header *)old_hole_loc;
   chunk_header->magic = HEAP_MAGIC;
   //set chunk as allocated
   chunk_header->allocated = 1;
   chunk_header->size = new_size;

   struct footer *chunk_footer = (struct footer *)(old_hole_loc + sizeof(struct header) + size);
   chunk_footer->magic = HEAP_MAGIC;
   chunk_footer->header = chunk_header;


   //TODO
   //check if old_hole_size - new_size > 0
   //if so, add new hole in that location.

   return (void *)((size_t) chunk_header+sizeof(struct header));
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
