// Memset implementation

#include "memset.h"
void *memset(void *s, int c, size_t n)
{
	size_t i = 0;
	for (i; i < n; i++){
		((unsigned char *)s)[i] = c;
	}
	return s;
}
