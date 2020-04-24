#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define UPBOUND 20
sem_t sem1;
sem_t sem2;

void *threadfunc1(void *pvoid)
{
	int i=0;
	while(i<UPBOUND)
	{
		sem_wait(&sem1);
		i++;
		printf("the integer of child1 thread is %d now.\n", i);
		sem_post(&sem2);
	}
}

void *threadfunc2(void *pvoid)
{
	int i=0;
	while(i<UPBOUND)
	{
		sem_wait(&sem2);
		i++;
		printf("the integer of child2 thread is %d now.\n", i);
		sem_post(&sem1);
	}
}

int main(int argc, char *argv[])
{
	pthread_t tid1, tid2;
	int i = sem_init(&sem1, 0, 5);
	printf("%d\n", i);
	sem_init(&sem2, 0, 5);
	printf("Semphores are initialized.\n");

	pthread_create(&tid1, 0, threadfunc1, NULL);
	pthread_create(&tid2, 0, threadfunc2, NULL);
	printf("Threads started.\n");

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Threads finished.\n");

	sem_destroy(&sem1);
	sem_destroy(&sem2);
	printf("Semphores destroied.\n");

	printf("Main thread finished\n");
	return 0;
}
