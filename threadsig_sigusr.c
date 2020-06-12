/*threadsig.c*/
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sighandler(int signo);

void * thr1_fn(void *arg)
{
	sigset_t    mask;
	int     rc, err;

	pthread_t   tid = pthread_self();
	printf("thread 1 with tid:%lu\n", tid);
    /*
	sigemptyset(&mask);

	sigaddset(&mask, SIGUSR1);
	err = pthread_sigmask(SIG_BLOCK, &mask, NULL);
    */

	rc = sleep(90);
	if (rc != 0)
		printf("thread 1... interrupted at %d second\n", 60 - rc);
	printf("thread 1 ends\n");
	return NULL;
}

void * thr2_fn(void *arg)
{
	sigset_t    mask;
	int     rc, err;

	pthread_t   tid = pthread_self();
	printf("thread 2 with tid:%lu\n", tid);
	sigemptyset(&mask); /* 初始化mask信号集 */

	sigaddset(&mask, SIGUSR2);
	err = pthread_sigmask(SIG_BLOCK, &mask, NULL);

	rc = sleep(90);

	if (rc != 0)
		printf("thread 2... interrupted at %d second\n", 60 - rc);
	printf("thread 2 ends\n");
	return NULL;
}

void * thr3_fn(void *arg)
{
	struct sigaction    action;
	action.sa_flags = 0;
	action.sa_handler = sighandler;

	sigaction(SIGINT, &action, NULL);

	pthread_t   tid = pthread_self();
	int     rc;

	printf("thread 3 with tid:%lu\n", tid);
	rc = sleep(90);
	if (rc != 0)
		printf("thread 1... interrupted at %d second\n", 60 - rc);
	printf("thread 3 ends\n");
	return NULL;
}

int main(void)
{
	int     rc, err;
	struct sigaction    action;
	pthread_t   thr1, thr2, thr3, thrm = pthread_self();

	action.sa_flags = SA_RESTART;
	action.sa_handler = sighandler;

	sigaction(SIGUSR1, &action, NULL);
	sigaction(SIGUSR2, &action, NULL);

	printf("thread main with pid %lu\n", getpid());
	err = pthread_create(&thr1, NULL, thr1_fn, NULL);
	if (err != 0) {
		printf("error in creating pthread:%d\t%s\n",err, strerror(rc));
		exit(1);
	}


	/*  pthread_kill(thr1, SIGALRM);    send a SIGARLM signal to thr1 before thr2 set the signal handler, then the whole process will be terminated*/
	err = pthread_create(&thr2, NULL, thr2_fn, NULL);
	if (err != 0) {
		printf("error in creating pthread:%d\t%s\n",err, strerror(rc));
		exit(1);
	}

	err = pthread_create(&thr3, NULL, thr3_fn, NULL);
	if (err != 0) {
		printf("error in creating pthread:%d\t%s\n",err, strerror(rc));
		exit(1);
	}

    printf("haha\n");
    int a = 0;
	while(1)
    {
        sleep(1);
        printf("main loop\n");
        if (a++ == 120) break;
    }
	printf("main ends\n");
	return 0;
}

void sighandler(int signo)
{
	pthread_t   tid = pthread_self();

	printf("thread with pid:%lu receive signo:%d\n", tid, signo);
	return;
}

