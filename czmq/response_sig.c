#include<stdio.h>
#include <signal.h>
#include<czmq.h>
#include<pthread.h>

zloop_t *gTiming_loop = NULL;
zsock_t *zsocket = NULL;
static int fpga_upgrade = 0;

static void sig_usr(int signum)
{
    if(signum == SIGUSR1)
    {
        printf("SIGUSR1 received\n");
    }
    else if(signum == SIGUSR2)
    {
        printf("SIGUSR2 received\n");
    }
    else
    {
        printf("signal %d received\n", signum);
    }
}

static int Timing_handle_AsyncMsg (zloop_t *loop, zsock_t *handle, void *arg)
{
    char Cmd_Str[256] = {0};

    char *string = NULL;
    string = zstr_recv(handle);
    printf("Recv: %s\n", string);
    sprintf(Cmd_Str, "reply to req %s", string);
    zstr_send(handle, Cmd_Str);
    zstr_free(&string);
    return 0;
}

static void misc_mgr_signal_handler (int signal_value)
{
    static bool flag = false;
   	int i = 0; 
    printf("timing catch SIGINT or SIGTERM, signal = %d", signal_value);
    if(!flag)
    {
        flag = true;
        zsock_destroy(&zsocket);
        zsocket = NULL;
        zloop_destroy (&gTiming_loop);
        exit(-1);
    }
}

void *Timing_Router_Task_Entry(void *arg)
{
    zsys_init();

    zsocket = zsock_new(ZMQ_REP);
    zsock_bind(zsocket, "tcp://*:8889");

    gTiming_loop = zloop_new();
    zsys_handler_set(misc_mgr_signal_handler);

    zloop_reader (gTiming_loop, zsocket, Timing_handle_AsyncMsg, NULL);
    zloop_reader_set_tolerant (gTiming_loop, zsocket);
    zloop_start (gTiming_loop);
    printf("zloop_start done");

    zsock_destroy(&zsocket);
    zloop_destroy (&gTiming_loop);
}

int main()
{
    char buf[20] = {0};
    int rc = 0;

    struct sigaction sa_usr1, sa_usr2;
    sa_usr1.sa_flags = SA_RESTART;
    sa_usr1.sa_handler = sig_usr;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr2.sa_flags = SA_RESTART;
    sa_usr2.sa_handler = sig_usr;
    sigemptyset(&sa_usr2.sa_mask);

    sigaction(SIGUSR1, &sa_usr1, NULL);
    sigaction(SIGUSR2, &sa_usr2, NULL);

    printf("My PID is %d\n", getpid());

    pthread_t ORURouter_Tid;
	rc = pthread_create(&ORURouter_Tid, NULL,(void *)Timing_Router_Task_Entry, NULL);
	if (rc) {
			printf("%s[%d], create Timing_Router_Task_Entry pthread failed", __FUNCTION__, __LINE__);
			return -1;
		}
    pthread_join(ORURouter_Tid, NULL);
    printf("main ends\n");
    return 0;
}

