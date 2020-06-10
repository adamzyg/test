#include<stdio.h>
#include <signal.h>
#include<czmq.h>

zloop_t *gTiming_loop = NULL;

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

int main()
{
    char buf[20] = {0};

    struct sigaction sa_usr;
    sa_usr.sa_flags = 0;
    sa_usr.sa_handler = sig_usr;   //信号处理函数

    sigaction(SIGUSR1, &sa_usr, NULL);
    sigaction(SIGUSR2, &sa_usr, NULL);

    printf("My PID is %d\n", getpid());

    zsys_init();

    zsock_t *socket = zsock_new(ZMQ_REP);
    zsock_bind(socket, "tcp://*:8889");

    gTiming_loop = zloop_new();
    zsys_handler_set(misc_mgr_signal_handler);

    zloop_reader (gTiming_loop, socket, Timing_handle_AsyncMsg, NULL);
    zloop_reader_set_tolerant (gTiming_loop, socket);
    zloop_start (gTiming_loop);

    zsock_destroy(&socket);
    zloop_destroy (&gTiming_loop);
    return 0;
}

