#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void * thr1_fn(void *arg)
{
	pthread_t   tid = pthread_self();
	printf("thread 1 with tid:%lu\n", tid);
	int rc = sleep(2);
	printf("thread 1 ends\n");
	return NULL;
}

int main(void)
{
	int     rc, err;
	pthread_t   thr1 = 0;
	pthread_t   thrm = pthread_self();

	printf("thread main with pid %lu\n",thrm);
	err = pthread_create(&thr1, NULL, thr1_fn, NULL);
	if (err != 0) {
		printf("error in creating pthread:%d\t%s\n",err, strerror(rc));
		exit(1);
	}
    sleep(5);
    rc = pthread_cancel(thr1);
    if (rc)
    {
        printf("cancel thread %lu failed\n", thr1);
    }
    return rc;
}


