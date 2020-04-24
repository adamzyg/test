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

# define LEN 100
pthread_key_t key;

void A(char *s)
{
	char *str = pthread_getspecific(key);
	strncpy(str, s, LEN);
}

void B()
{
	char *str = pthread_getspecific(key);
	printf("%s\n", str);
}

void destructor(void *ptr)
{
	free(ptr);
	printf("key memory free!\n");
}

void *threadfunc1(void *pvoid)
{
	pthread_setspecific(key, malloc(LEN));
	A("thread1");
	B();
}

void *threadfunc2(void *pvoid)
{
	pthread_setspecific(key, malloc(LEN));
	A("thread2");
	B();
}

int main()
{
	pthread_t tid1, tid2;
	pthread_key_create(&key, destructor);
	pthread_create(&tid1, NULL, &threadfunc1, NULL);
	pthread_create(&tid2, NULL, &threadfunc2, NULL);
	pthread_exit(NULL);

	return(0);
}
