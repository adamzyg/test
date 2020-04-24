#include<stdio.h>
#include<string.h>
#include<pthread.h>
#define NUMTHREADS 10

static void *printarg(void *arg)
{
	fprintf(stderr, "Thread received %d\n", *(int *)arg);
	return NULL;
}

int main()
{
	int error;
	int i;
	int j;
	pthread_t tid[NUMTHREADS];

	for(i=0; i<NUMTHREADS; i++)
	{
		if(error = pthread_create(tid+1, NULL, printarg, (void *)&i))
		{
			fprintf(stderr, "Failed to create thread: %s\n", strerror(error));
			tid[i] = pthread_self();
		}
	}
	sleep(1);

	for(j=0; j<NUMTHREADS; j++)
	{
		if(pthread_equal(tid[j], pthread_self()))
		{
			continue;
		}

		if(error = pthread_join(tid[j], NULL))
		{
			fprintf(stderr, "Failed to join thread: %s\n", strerror(error));
		}
	}

	printf("All threads done\n");
	return 0;
}
