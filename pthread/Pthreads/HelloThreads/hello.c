#include <stdio.h>
#include <pthread.h>

#define NUMTHREADS 4


void *helloFunc(void *pArg) 
{ 
   printf("Hello Thread\n"); 
}

main() 
{	
   pthread_t hThread[NUMTHREADS];

   for (int i = 0; i < NUMTHREADS; i++)
       pthread_create(&hThread[i], NULL, helloFunc, NULL);

   for (int j = 0; j < NUMTHREADS; j++)
       pthread_join(hThread[j], NULL);

   return 0; 
}

