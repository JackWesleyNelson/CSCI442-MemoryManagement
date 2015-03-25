// REQUIRED-1: Memset is able to return the proper pointer

#include "../test.h"
#include "../../memset.h"
#include "../../memset.c"

#define MEM_SIZE 100

int main(int argc, char **argv)
{
   char memory[MEM_SIZE];
   void *returned;

   returned = memset(memory, 0, MEM_SIZE);
   t_assert("A correct pointer should be returned", returned == memory);

   returned = memset(memory, 5, 3);
   t_assert("A correct pointer should be returned", returned == memory);

   returned = memset(memory + 3, 5, 3);
   t_assert("A correct pointer should be returned", returned == memory + 3);

   return 0;
}
