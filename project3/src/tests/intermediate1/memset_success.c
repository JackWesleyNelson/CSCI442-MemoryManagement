// REQUIRED-5: Memset works correctly

#include "../test.h"
#include "../../memset.h"
#include "../../memset.c"

#define MEM_SIZE 100

int main(int argc, char **argv)
{
   char memory[MEM_SIZE];
   int i;

   memset(memory, 0, MEM_SIZE);

   for(i = 0; i < MEM_SIZE; i++)
   {
      t_assert("The memory region should be set correctly", memory[i] == 0);
   }

   memset(memory, 5, MEM_SIZE);

   for(i = 0; i < MEM_SIZE; i++)
   {
      t_assert("The memory region should be set correctly", memory[i] == 5);
   }

   return 0;
}
