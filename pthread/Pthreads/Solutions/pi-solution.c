#include <stdio.h>
#include <pthread.h>

#define NUM_STEPS 1000000
#define NUM_THREADS 4

double gStep = 0.0;
double gPi = 0.0;
pthread_mutex_t gLock;

void *threadFunction(void *pArg)
{
   int myNum = *((int *)pArg);
   double partialSum = 0.0, x;  // local to each thread

   for ( int i=myNum; i<NUM_STEPS; i+=NUM_THREADS )  // use every NUM_THREADS-th step
   {
      x = (i + 0.5f) / NUM_STEPS;
      partialSum += 4.0f / (1.0f + x*x);  //compute partial sums at each thread
   }

   pthread_mutex_lock(&gLock);
     gPi += partialSum * gStep;  // add partial to global final answer
   pthread_mutex_unlock(&gLock);

}

main()
{
   pthread_t threadHandles[NUM_THREADS];
   int tNum[NUM_THREADS];

   printf("Computed value of Pi: ");

   pthread_mutex_init(&gLock, NULL);
   gStep = 1.0 / NUM_STEPS;
   for ( int i=0; i<NUM_THREADS; ++i )
   {
      tNum[i] = i;
      pthread_create(&threadHandles[i],  // Thread handle
                     NULL,               // Attributes
                     threadFunction,     // Thread function
                     (void *) &tNum[i]); // paramemter for thread func()
   }

   for ( int j=0; j<NUM_THREADS; ++j )
   {
      pthread_join(threadHandles[j], NULL);
   }

   pthread_mutex_destroy(&gLock);

   printf("%12.9f\n", gPi );
}
