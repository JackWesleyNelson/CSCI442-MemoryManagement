#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define t_assert(message, test) do { if(!(test)) { fprintf(stderr, "Line %d of %s: %s\n", __LINE__, __FILE__,  message); return 1; } } while (0)

#endif
