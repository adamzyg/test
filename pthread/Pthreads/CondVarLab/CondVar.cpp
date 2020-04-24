/*
   This simple code demonstrates Pthreads condition variables

   This code computes sum of Mercator's series.

   Mercator's series expression for ln( x + 1 ) function is:
 
      ln( x + 1 ) = x - x^2/2 + x^3/3 - x^4/4 + x^5/5 - ...

      for -1 < x <= 1.
*/

#include "pthread.h"
#include <stdio.h>      // Header file for standard file i/o.
//#include <conio.h>
#include <math.h>
#include <unistd.h>
       
#define NUM_THREADS 4
#define SERIES_MEMBER_COUNT 50000

//arrray of threads structures
pthread_t g_threads[NUM_THREADS];
pthread_t g_master_thread;

pthread_mutex_t g_mutex_CS;

double sums[NUM_THREADS];
double x = 1.0, res = 0.0;
int threadNum = 0;
int tPause = 1;

double getMember(int n, double x)
{
   double numerator = 1;
   for( int i=0; i<n; i++ )
      numerator = numerator*x;

   if ( n % 2 == 0 )
      return ( - numerator / n );
   else
      return numerator/n;
}

void *threadProc(void* vpar)
{
   int threadIndex = *((int *) vpar);
   while (tPause) {sleep(1);}  // spin wait for "master" thread
   sums[threadIndex] = 0;
   for(int i=threadIndex; i<SERIES_MEMBER_COUNT;i+=NUM_THREADS)
      sums[threadIndex] += getMember(i+1, x);

   //Signal Master thread that one more processing thread is done
   pthread_mutex_lock( &g_mutex_CS);
     threadNum++;
   pthread_mutex_unlock( &g_mutex_CS);

   delete vpar;

   return 0;
}


void*  masterThreadProc(void  *par)
{

   tPause = 0; // allow worker threads to proceed

   while (threadNum != NUM_THREADS)  {sleep(1);} // spin wait

   res = 0;
   for(int i=0; i<NUM_THREADS; i++)
      res += sums[i];
   return 0;
}

int main()
{
   threadNum = 0;
   pthread_mutex_init( &g_mutex_CS, NULL );
   
   
   for(int i=0; i<NUM_THREADS;i++)
   {
      int *threadIdPtr = new int;
      *threadIdPtr = i;
   
      int ret = pthread_create(&g_threads[i],NULL, threadProc,threadIdPtr);
      if(ret)
         return 0;
   }

   int ret = pthread_create(&g_master_thread,NULL, masterThreadProc, NULL);
   if(ret)
      return 0;
   
   printf("Count of ln(1 + x) Mercator's series members is %d\n",SERIES_MEMBER_COUNT);
   printf("Argument value of x is %f\n", (double)x);

   pthread_join( g_master_thread, NULL);

   printf("\n                          Result is %10.8f\n", res);
   printf("By function call ln(1 + %8.6f) = %10.8f\n",x, log(1+x));
   printf("\n\n");
}
