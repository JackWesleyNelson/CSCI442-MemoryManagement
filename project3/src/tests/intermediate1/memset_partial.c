// REQUIRED-2: Memset is able to operate on different sized regions

#include "../test.h"
#include "../../memset.h"
#include "../../memset.c"

#define MEM_SIZE 100

int main(int argc, char **argv)
{
   char memory[MEM_SIZE];
   int i;

   memset(memory, 5, MEM_SIZE);

   memset(memory, 1, 5);

   for(i = 0; i < 5; i++)
   {
      t_assert("The memory region should be set correctly", memory[i] == 1);
   }
   for(i = 5; i < MEM_SIZE; i++)
   {
      t_assert("The memory region should not have been set", memory[i] == 5);
   }

   memset(memory, 5, MEM_SIZE);

   memset(memory + 5, 1, 10);

   for(i = 0; i < 5; i++)
   {
      t_assert("The memory region should not have been set", memory[i] == 5);
   }
   for(i = 5; i < 15; i++)
   {
      t_assert("The memory region should be set correctly", memory[i] == 1);
   }
   for(i = 15; i < MEM_SIZE; i++)
   {
      t_assert("The memory region should not have been set", memory[i] == 5);
   }

   return 0;
}
