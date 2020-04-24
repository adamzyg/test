#include <stdio.h>
#include <pthread.h>

#define NUMTHREADS 4


void *helloFunc(void *pArg) 
{ 
   int myNum = *((int *)pArg);

   printf("Hello from Thread #%d\n", myNum); 
}

main() 
{	
   pthread_t hThread[NUMTHREADS];
   int tNum[NUMTHREADS];

   for (int i = 0; i < NUMTHREADS; i++) {
       tNum[i] = i;
       pthread_create(&hThread[i], NULL, helloFunc, &tNum[i]);
   }

   for (int j = 0; j < NUMTHREADS; j++)
       pthread_join(hThread[j], NULL);

   return 0; 
}

