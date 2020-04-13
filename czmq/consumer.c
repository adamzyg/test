#include<stdio.h>
#include<czmq.h>


zloop_t *gTiming_loop = NULL;

static int Timing_handle_AsyncMsg (zloop_t *loop, zsock_t *handle, void *arg)
{

    char *string = NULL;
    string = zstr_recv(handle);
    printf("Recv: %s\n", string);
    zstr_free(&string);

    //zmsg_t *Cmd_Msg;
    //zframe_t *O_Identity;
    //zframe_t *O_Content;
    //size_t Frame_Len;
    //char *Frame_Data = NULL;
    //char Cmd_Str[256];
    //memset(Cmd_Str, 0, sizeof(Cmd_Str));

    //if(NULL != Cmd_Msg){
    //    O_Identity = zmsg_pop(Cmd_Msg);
    //    assert(O_Identity);
    //    O_Content = zmsg_pop(Cmd_Msg);
    //    assert(O_Content);
    //    Frame_Len = zframe_size(O_Content);
    //    Frame_Data = zframe_data(O_Content);
    //    memcpy(Cmd_Str, Frame_Data, Frame_Len);
    //    printf("recv asyn event:%s\n",Cmd_Str);
    //    //timing_socket_rec(O_Identity, Cmd_Str);
    //    zframe_destroy(&O_Identity);
    //    zframe_destroy(&O_Content);
    //    zmsg_destroy(&Cmd_Msg);
    //}	

    return 0;
}

int main()
{
    char *string = NULL;
    zsys_init();
    zsock_t *socket = zsock_new(ZMQ_PULL);
    zsock_connect(socket, "tcp://127.0.0.1:8888");
    //while((string = zstr_recv(socket)) != NULL)
    //{
    //    printf("Recv: %s\n", string);
    //    zstr_free(&string);
    //}
    gTiming_loop = zloop_new();
    zloop_reader (gTiming_loop, socket, Timing_handle_AsyncMsg, NULL);
    zloop_reader_set_tolerant (gTiming_loop, socket);
    zloop_start (gTiming_loop);

    zsock_destroy(&socket);
    return 0;
}

