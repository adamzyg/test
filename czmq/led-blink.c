#include <unistd.h>
#include <stdio.h>
#include <czmq.h>


int main(int argc, char *argv[])
{
    char buf[64] = {0};
    char *request_pattern = "{\"cmd\": %d, \"cpu\": %d}";
    if (argc != 4)
    {
        printf("%s on/off <slot> <cpu_id>\n", argv[0]);
        printf("exmaple: %s on 0 0\n", argv[0]);
        return -1;
    }

    int slot = atoi(argv[2]);
    if (slot != 0 && slot != 1)
    {
        printf("%s on/off <slot> <cpu_id>\n", argv[0]);
        printf("exmaple: %s on 0 0\n", argv[0]);
        return -1;
    }

    int cpu = atoi(argv[3]);
    if (cpu != 0 && cpu != 1)
    {
        printf("%s on/off <slot> <cpu_id>\n", argv[0]);
        printf("exmaple: %s on 0 0\n", argv[0]);
        return -1;
    }

    zsys_init();

    zsock_t *req_socket = zsock_new(ZMQ_REQ);
    if (slot == 0)
    {
        zsock_connect(req_socket, "tcp://192.168.69.10:12350");
    }
    else
    {
        zsock_connect(req_socket, "tcp://192.168.69.20:12350");
    }

    if (!strcmp("on", argv[1]))
    {
        sprintf(buf, request_pattern, 0x12, cpu);
    }
    else if (!strcmp("off", argv[1]))
    {
        sprintf(buf, request_pattern, 0x13, cpu);
    }
    else
    {
        printf("%s on/off <slot> <cpu_id>\n", argv[0]);
        printf("exmaple: %s on 0 0\n", argv[0]);
        zsock_destroy(&req_socket);
        return -1;
    }

    zstr_send(req_socket, buf);
    char *string = zstr_recv(req_socket);
    printf("return: %s\n", string);
    zstr_free(&string);

    zsock_destroy(&req_socket);

    return 0;
}
