#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __USE_UNI98

#define THREADCOUNT 10

pthread_rwlock_t rwlock;

void *reader(void *pvoid)
{
	pthread_rwlock_rdlock(&rwlock);
	printf("reader%d worked.\n", (int)pvoid);
	if(pthread_rwlock_unlock(&rwlock))
	{
		printf("reader%d unlock error!\n", (int)pvoid);
	}
	return NULL;
}

void *writer(void *pvoid)
{
	pthread_rwlock_wrlock(&rwlock);
	printf("writer%d worked.\n", (int)pvoid);
	if(pthread_rwlock_unlock(&rwlock))
	{
		printf("writer%d unlock error!\n", (int)pvoid);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t rw_pthread_id[THREADCOUNT];
	pthread_attr_t threadattr;
	int i, rand;
	int readercount = 1, writercount =1;
	int halfmax = RAND_MAX/2;

	if(pthread_rwlock_init(&rwlock, NULL))
	{
		printf("initialize rwlock error!\n");
	}

	pthread_attr_init(&threadattr);
	pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);

	pthread_rwlock_wrlock(&rwlock);
	for(i=0; i<THREADCOUNT; i++)
	{
		rand = random();
		if(rand < halfmax)
		{
			pthread_create(&rw_pthread_id[i], &threadattr, &reader, (void*)readercount);
			printf("Create reader%d\n", readercount++);
		}
		else
		{
			pthread_create(&rw_pthread_id[i], &threadattr, &writer, (void*)writercount);
			printf("Create writer%d\n", writercount++);
		}
	}
	pthread_rwlock_unlock(&rwlock);
	pthread_exit(NULL);
	return 0;
}
