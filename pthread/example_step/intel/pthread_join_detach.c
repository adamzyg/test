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

void *threadfunc(void *pvoid)
{
	int id = (int)pvoid;
	printf("child thread%d says:Hello world!\n", id);
	return NULL;
}

int main()
{
	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, threadfunc, 1);
	pthread_create(&tid2, NULL, threadfunc, 2);

	pthread_detach(tid1);
//	pthread_join(tid2, NULL);
	printf("Main process says:Hello world!\n");
	pthread_exit(NULL);
	return 0;
}
