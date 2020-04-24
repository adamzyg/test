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
#include <stdlib.h>
#include <string.h>

int i = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

void *threadfunc(void *pvoid)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(i<200)
		{
			i++;
			pthread_cond_signal(&condvar);
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			pthread_mutex_unlock(&mutex);
			break;
		}
	}
	return NULL;
}

int main()
{
	pthread_t tid;
	pthread_create(&tid, NULL, threadfunc, NULL);

	pthread_mutex_lock(&mutex);
	while(i<100)
	{
		pthread_cond_wait(&condvar, &mutex);
	}
	printf("i = %d\n", i);
	pthread_mutex_unlock(&mutex);
	pthread_join(tid, NULL);
	pthread_cond_destroy(&condvar);
	pthread_mutex_destroy(&mutex);

	return 0;

}

