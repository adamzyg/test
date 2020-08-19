#include<stdio.h>
#include<czmq.h>

int main()
{
    char buf[20] = {0};

    //zactor_t *auth = zactor_new(zauth,NULL);
    //
    ////  Get some indication of what the authenticator is deciding
    //zstr_send (auth,"VERBOSE");
    //zsock_wait (auth);
    //
    ////  Whitelist our address; any other address will be rejected
    ////  Add as much address as argument as you like before the NULL-Argument
    ////  e.g. zstr_sendx (auth,"ALLOW","127.0.0.1","192.168.1.20",NULL);
    //zstr_sendx (auth,"ALLOW","127.0.0.1",NULL);    
    //zsock_wait (auth);
    zsys_init();

    zsock_t *socket = zsock_new(ZMQ_PUSH);
    zsock_connect(socket, "tcp://127.0.0.1:8888");
    for(int i=1; i<=10; i++)
    {
        sprintf(buf, "%d: Hello, World", i);
        zstr_send(socket, buf);
        sleep(1);
    }
    //zactor_destroy (&auth);
    zsock_destroy(&socket);
    return 0;
}

