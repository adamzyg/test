/**************************************************/
/* Another thread example.  This one shows that   */
/* pthreads in Linux can use both processors in   */
/* a dual-processor Pentium.                      */
/*                                                */
/* Usage:  a.out <num threads>                    */
/*                                                */
/* To compile me in Linux type:                   */
/*    gcc -o another another.c -lpthread          */
/**************************************************/

#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 10

int sum; /* this data is shared by the thread(s) */
void *runner(void * param);

main(int argc, char *argv[])
{
  int num_threads, i;
  pthread_t tid[MAX_THREADS];     /* the thread identifiers  */
  pthread_attr_t attr; /* set of thread attributes */

  if (argc != 2) {
    fprintf(stderr, "usage:  a.out <integer value>\n");
    exit();
  }

  if (atoi(argv[1]) <= 0) {
    fprintf(stderr,"%d must be > 0\n", atoi(argv[1]));
    exit();
  }

  if (atoi(argv[1]) > MAX_THREADS) {
    fprintf(stderr,"%d must be <= %d\n", atoi(argv[1]), MAX_THREADS);
    exit();
  }

  num_threads = atoi(argv[1]);

  /* get the default attributes */
  pthread_attr_init(&attr);

  /* create the threads */
  for (i=0; i<num_threads; i++) {
    pthread_create(&tid[i], &attr, runner, NULL); 
  }

  /* now wait for the threads to exit */
  for (i=0; i<num_threads; i++) {
    pthread_join(tid[i],NULL);
  }
}

/* The thread will begin control in this function */
void *runner(void * param)
{
  int i;
  while(1) {
    i++ % 30000;
    sum = i;
  }
  pthread_exit(0);
}
