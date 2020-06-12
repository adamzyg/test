#include<stdio.h>
#include <signal.h>
#include<czmq.h>

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
int main()
{
    char buf[20] = {0};

    struct sigaction sa_usr;
    sa_usr.sa_flags = SA_RESTART;
    sa_usr.sa_handler = sig_usr;   //信号处理函数
    sigemptyset(&sa_usr.sa_mask);

    sigaction(SIGUSR1, &sa_usr, NULL);
    sigaction(SIGUSR2, &sa_usr, NULL);

    printf("My PID is %d\n", getpid());

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
    return 0;
}

