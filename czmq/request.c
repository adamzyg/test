#include<stdio.h>
#include<czmq.h>
#include<pthread.h>

static zsock_t *req_socket = NULL;
pthread_t ORUCli_Tid;
zloop_t *gTiming_loop = NULL;

static int Timing_handle_AsyncMsg (zloop_t *loop, zsock_t *handle, void *arg)
{
    char *string = NULL;
    string = zstr_recv(handle);
    printf("RESPONSE Recv: %s\n", string);
    zstr_free(&string);
    return 0;
}

void *myptread_send()
{
    char buf[20] = {0};
    char *string = NULL;

    for(int i=1; i<=10; i++)
    {
        sprintf(buf, "%d: Hello, World", i);
        zstr_send(req_socket, buf);
        string = zstr_recv(req_socket);
        printf("RESPONSE Recv: %s\n", string);
        zstr_free(&string);
        sleep(1);
    }
    return NULL ;
}

int main()
{
    void *status;
    zsys_init();

    req_socket = zsock_new(ZMQ_REQ);
    zsock_connect(req_socket, "tcp://127.0.0.1:8889");
    pthread_create(&ORUCli_Tid, NULL, (void *)myptread_send, NULL);

    //gTiming_loop = zloop_new();
    //zloop_reader (gTiming_loop, req_socket, Timing_handle_AsyncMsg, NULL);
    //zloop_reader_set_tolerant (gTiming_loop, req_socket);
    //zloop_start (gTiming_loop);


    pthread_join(ORUCli_Tid, &status);
    zsock_destroy(&req_socket);
    //zloop_destroy(&gTiming_loop);
    return 0;
}

