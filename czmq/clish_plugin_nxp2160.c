#include <stdlib.h>
#include <stdio.h>
#include <string.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  

#include <clish/plugin.h>
#include <clish/shell.h>
#include <clish/pargv.h>

#include <czmq.h>


#define NXP2160_NUM  2
#define SOCK_PORT 10300
static zsock_t *nxp2160_cli_sock = NULL;
static const char *resp_end = "<DONE>";
unsigned int ip_bytes[4] = {0};
typedef struct {
    zsock_t *cli_sock;
    char *nxp2160_ip;
}NXP2160_CLI_T;
NXP2160_CLI_T nxp2160Cli_t[NXP2160_NUM] = {
    {.cli_sock = NULL, .nxp2160_ip = "192.168.10.11"},
    {.cli_sock = NULL, .nxp2160_ip = "192.168.20.11"},
};
static int selectNxp2160Num = 0;
static int clish_plugin_nxp2160_cli_handler(const clish_command_t *cmd, clish_pargv_t *argv);

CLISH_PLUGIN_SYM(clish_plugin_nxp2160_cli_fn)
{
    int rc = -1;
    /*get command and argv from klish*/
    clish_context_t *cli_context = (clish_context_t *)clish_context;
    clish_pargv_t *argv = clish_context__get_pargv(cli_context);
    const clish_command_t *cmd = clish_context__get_cmd(cli_context);

    /*reslove and transfer command*/
    rc = clish_plugin_nxp2160_cli_handler(cmd, argv);
    return rc;
}

CLISH_PLUGIN_SYM(clish_plugin_select_2160_handler)
{
    int rc = -1;
    int i;
    const char *nxp2160_val = NULL;
    clish_context_t *pctx = (clish_context_t *)clish_context;
    clish_pargv_t *argv = clish_context__get_pargv(pctx);
    int argc = clish_pargv__get_count(argv);
    for (i = 0; i < argc; i++) {
        clish_parg_t *arg = clish_pargv__get_parg(argv, i);
        if (!arg) {
            fprintf(stderr, "argv[%d] is null\n", i);
            return -1;
        }
        nxp2160_val = clish_parg__get_value(arg);
    }
    selectNxp2160Num = atoi(nxp2160_val);
    fprintf(stdout, "   Enter cig 2160[%d] cli view ...\n", selectNxp2160Num);
    return 0;
}

CLISH_PLUGIN_FINI(nxp2160_fini)
{
    int i;
    for(i = 0; i < NXP2160_NUM; i++){	
        if (nxp2160Cli_t[i].cli_sock )
            zsock_destroy(&nxp2160Cli_t[i].cli_sock );
    }
    return 0;
}

CLISH_PLUGIN_INIT(nxp2160_cli)
{
    clish_plugin_add_fini(plugin, nxp2160_fini);
    clish_plugin_add_sym(plugin, clish_plugin_nxp2160_cli_fn, "nxp2160_cli_handler");
    clish_plugin_add_sym(plugin, clish_plugin_select_2160_handler, "select_2160_handler");

    int rc = -1;
    for(int i = 0;i < NXP2160_NUM; i++){
        nxp2160Cli_t[i].cli_sock  = zsock_new(ZMQ_DEALER);
        if (!nxp2160Cli_t[i].cli_sock) {
            fprintf(stderr, "fail to new cli socket");
            return -1;
        }
        rc = zsock_connect(nxp2160Cli_t[i].cli_sock , "tcp://%s:%d", nxp2160Cli_t[i].nxp2160_ip, SOCK_PORT);
        if (rc) {
            fprintf(stderr, "the endpoint: %s is invalid", nxp2160Cli_t[i].nxp2160_ip);
            return -1;
        }
        //fprintf(stdout, "connect the : tcp://%s:%d\n", nxp2160Cli_t[i].nxp2160_ip, SOCK_PORT);
    }

    return 0;
}

static int clish_plugin_nxp2160_cli_handler(const clish_command_t *cmd, clish_pargv_t *argv)
{
    int argc = clish_pargv__get_count(argv);
    int i, rc;
    FILE *fp = NULL;
    char *send_buf, *str, *p;
    size_t ulen;
    ssize_t slen;
    int flow_flag = 0;

    zmsg_t *msg;
    zframe_t *content;

    //fprintf(stdout, "DEBUG: enter into clish_plugin_nxp2160_cli_handler\n");
    fp = open_memstream(&send_buf, &ulen);
    if (!fp)
    {
        fprintf(stderr, "DEBUG: open_memstream error\n");
        return -1;
    }

    const char *cmdname= clish_command__get_name(cmd);

    fprintf(stdout, "DEBUG: cmdname %s\n", cmdname);
    fprintf(fp, "%s", cmdname);
    for (i = 0; i < argc; i++) {
        clish_parg_t *arg = clish_pargv__get_parg(argv, i);
        if (!arg) {
            fprintf(stderr, "argv[%d] is null\n", i);
            return -1;
        }

        const char *name = clish_parg__get_name(arg);
        if (!name) {
            fprintf(stderr, "argv[%d] name is null\n", i);
            return -1;
        }

        if (strstr(name,"item")){
            continue;
        }

        fprintf(fp, " %s", clish_parg__get_value(arg));
    }
    fprintf(fp, "\n");
    fclose(fp);
    fprintf(stdout, "DEBUG: buf: %s\n", send_buf);

    nxp2160_cli_sock = nxp2160Cli_t[selectNxp2160Num - 1].cli_sock;
    msg = zmsg_new();
    content = zframe_new(send_buf, strlen(send_buf));
    free(send_buf);
    zmsg_add(msg, content);

    rc = zmsg_send(&msg, nxp2160_cli_sock);
    if (rc) {
        zmsg_destroy(&msg);
        perror("zstr_send");
        return -1;
    }   

    zpoller_t *poller = zpoller_new(nxp2160_cli_sock, NULL);
    assert(poller);
    while (1) {
        zsock_t *which = zpoller_wait(poller, 60 * 60 * 1000);
        if (!which) {
            if (zpoller_expired(poller)) {
                fprintf(stdout, "timeout\n");
                zpoller_destroy(&poller);
                return -1;
            }

            if (zpoller_terminated(poller)) {
                fprintf(stdout, "Interrupted or context was destoryed\n");
                zpoller_destroy(&poller);
                return -1;
            }
        }
        else {
            if (which == nxp2160_cli_sock) {
                msg = zmsg_recv(nxp2160_cli_sock);

                if (!msg) {
                    perror("rcv_msg");
                    return -1;
                }
                str = zmsg_popstr(msg);
                p = strstr(str, resp_end);

                if (p) {
                    *p = '\0';
                    fprintf(stdout, "%s", str);
                    free(str);
                    zmsg_destroy(&msg);
                    zpoller_destroy(&poller);

                    return 0;
                } 
                else {
                    fprintf(stdout, "%s", str);
                    free(str);
                    zmsg_destroy(&msg);
                }        
            }
        }
    }

    return 0;
}
