// Memset implementation

#include "memset.h"

void *memset(void *s, int c, size_t n)
{
   // TODO: IMPLEMENT THIS FUNCTION
   for(int i = s; i < n; i++){
      s*[i] = c;
   }
   return s;
}
