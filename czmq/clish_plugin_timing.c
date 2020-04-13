#include <stdlib.h>
#include <stdio.h>
#include <string.h>  
#include <errno.h>  
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>
#include <netinet/in.h>  

#include <clish/plugin.h>
#include <clish/shell.h>
#include <clish/pargv.h>

#include <czmq.h>
#include <cJSON.h>

#define MAXLINE 4096  


#define MSG_CLI_LMK05028_REG_READ           0x01
#define MSG_CLI_LMK05028_REG_WRITE          0x02
#define MSG_CLI_LMK05028_CFG_LOAD           0x03
#define MSG_CLI_GET_TIMING_STATUS           0x04

#define MSG_CLI_ZENQ_REG_READ               0x05
#define MSG_CLI_ZENQ_REG_WRITE              0x06

#define MSG_CLI_PHY_REG_READ                0x07
#define MSG_CLI_PHY_REG_WRITE               0x08

#define MSG_CLI_LMK05028_EEPROM_READ        0x09

#define MSG_CLI_LMK05028_SRAM_READ          0x0a
#define MSG_CLI_LMK05028_FSM_OP             0x0f


typedef struct timing_state_s {
    unsigned char   ucFsm;
    unsigned char   ucCurrentSource;
    unsigned char   ucFefClockStates[3];
    unsigned char   ucRes[2];
} timing_state_t;



typedef struct adi_timing_state_s
{
    unsigned char    sysclk3001h;
    unsigned char    ref3005_8h[4];       //REFA status; REFAA status; REFB status; REFBB status 
    unsigned char    dpll0Lock3100h;
    unsigned char    dpll1Lock3200h;
} adi_timing_state_t;



char *localhost = "127.0.0.1";
unsigned int port = 8003;
static zsock_t *timing_cli_sock = NULL;
static const char *resp_end = "<DONE>";
unsigned int ip_bytes[4] = {0};

int htoi(const char s[])  
{  
    int i;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
    {  
        i = 2;  
    }  
    else  
    {  
        i = 0;  
    }  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
    {  
        if (tolower(s[i]) > '9')  
        {  
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        }  
        else  
        {  
            n = 16 * n + (tolower(s[i]) - '0');  
        }  
    }  
    return n;  
}  

int clish_plugin_timing(const clish_command_t *cmd, clish_pargv_t *argv);

CLISH_PLUGIN_SYM(timing_fn)
{
    int rc = -1;
    /*get command and argv from klish*/
    clish_context_t *cli_context = (clish_context_t *)clish_context;
    clish_pargv_t *argv = clish_context__get_pargv(cli_context);
    const clish_command_t *cmd = clish_context__get_cmd(cli_context);

    /*reslove and transfer command*/
    rc = clish_plugin_timing(cmd, argv);
    return rc;
}

CLISH_PLUGIN_FINI(timing_fini)
{
    if (timing_cli_sock)
        zsock_destroy(&timing_cli_sock);
    return 0;
}

CLISH_PLUGIN_INIT(timing)
{
    int rc = 0;
    clish_plugin_add_fini(plugin, timing_fini);
    clish_plugin_add_sym(plugin, timing_fn, "timing");

    timing_cli_sock  = zsock_new(ZMQ_DEALER);
    if (!timing_cli_sock) {
        fprintf(stderr, "fail to new cli socket");
        return -1;
    }
    rc = zsock_connect(timing_cli_sock , "tcp://%s:%d", localhost, port);
    if (rc) {
        fprintf(stderr, "the endpoint: %s is invalid", localhost);
        return -1;
    }

    return 0;
}

void printJson(cJSON * root)
{
    if (!root)
    {
        return;
    }

    int i = 0;
    for(; i<cJSON_GetArraySize(root); i++)
    {
        cJSON * item = cJSON_GetArrayItem(root, i);
        if(cJSON_Object == item->type)
            printJson(item);
        else
        {
            printf("%s: ", item->string);
            printf("%s\n", cJSON_Print(item));
        }
    }
}

int clish_handle_cmd(char *send_buf)
{
    int rc = 0;
    char *str, *p;
    zmsg_t *msg;
    zframe_t *content;

    cJSON * root = NULL;

    msg = zmsg_new();
    content = zframe_new(send_buf, strlen(send_buf));
    zmsg_add(msg, content);

    rc = zmsg_send(&msg, timing_cli_sock);
    if (rc) {
        zmsg_destroy(&msg);
        perror("zstr_send");
        return -1;
    }   

    zpoller_t *poller = zpoller_new(timing_cli_sock, NULL);
    assert(poller);
    while (1) {
        zsock_t *which = zpoller_wait(poller, 10 * 1000);
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
            if (which == timing_cli_sock) {
                msg = zmsg_recv(timing_cli_sock);

                if (!msg) {
                    perror("rcv_msg");
                    return -1;
                }
                str = zmsg_popstr(msg);
                p = strstr(str, resp_end);

                if (p) {
                    *p = '\0';
                    fprintf(stdout, "%s", str);
                    root = cJSON_Parse(str);     
                    if (!root) 
                    {
                        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
                    }
                    printJson(root);
                    if (root)
                    {
                        cJSON_Delete(root);
                        root = NULL;
                    }
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

const char *regread_pattern = "{\"cmd\": %d, \"address\": %d}";
const char *regwrite_pattern = "{\"cmd\": %d, \"address\": %d, \"value\": %d}";
const char *status_pattern = "{\"cmd\": %d}";
const char *ramread_pattern = "{\"cmd\": %d, \"address\": %d}";
const char *romread_pattern = "{\"cmd\": %d, \"address\": %d}";
const char *zenqread_pattern = "{\"cmd\": %d, \"address\": %d}";
const char *fsm_pattern = "{\"cmd\": %d, \"op\": \"%s\"}";
const char *phyread_pattern = "{\"cmd\": %d,  \"phy\": %d, \"device\": %d, \"address\": %d}";
const char *phywrite_pattern = "{\"cmd\": %d,  \"phy\": %d, \"device\": %d, \"address\": %d, \"value\": %d}";
const char *cfgload_pattern = "{\"cmd\": %d,  \"path\": \"%s\"}";
int clish_plugin_timing(const clish_command_t *cmd, clish_pargv_t *argv)
{
    int rc = 0;
    int i = 0;
    int argc = 0;
    clish_parg_t *arg;
    const char *cmdname;
    const char *pargvname;
    char buf[MAXLINE] = {0};  
    int rec_len = 0;  
    unsigned cmd_code = 0x0;
    char send_buf[256] = {0};

    unsigned short usRegOffset = 0;
    unsigned short usSramOffset = 0;
    unsigned short usEepromOffset = 0;

    unsigned hexvalue = 0x0;

    cmdname= clish_command__get_name(cmd);
    if(!cmdname){
        fprintf(stderr, "command name is null\n");
        return -1;
    }
    //printf("cmdname: #%s#\n", cmdname);

    if (!strcmp(cmdname, "regread"))
    {
        const char *pargvregvalue;
        cmd_code = MSG_CLI_LMK05028_REG_READ;

        argc = clish_pargv__get_count(argv);
        for (i = 0; i < argc; i++) {
            arg = clish_pargv__get_parg(argv, i);
            if (!arg) {
                fprintf(stderr, "argv[%d] is null\n", i);
                return -1;
            }

            pargvname = clish_parg__get_name(arg);
            if (!pargvname) {
                fprintf(stderr, "argv[%d] name is null\n", i);
                return -1;
            }

            if (!strcmp(pargvname, "reg-address"))
            {
                pargvregvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvregvalue);
            }
            else
            {
                printf("Error: command %s parameter %s is invalid, should be %s.\n", cmdname, pargvname, "reg-address");
                return -1;
            }

        }
        usRegOffset = htoi(pargvregvalue);
        sprintf(send_buf, regread_pattern, cmd_code, usRegOffset);
    }
    else if (strcmp (cmdname, "regwrite") == 0)
    {
        const char *pargvregvalue;
        const char *pargvhexvalue;
        cmd_code = MSG_CLI_LMK05028_REG_WRITE;

        argc = clish_pargv__get_count(argv);
        for (i = 0; i < argc; i++) {
            arg = clish_pargv__get_parg(argv, i);
            if (!arg) {
                fprintf(stderr, "argv[%d] is null\n", i);
                return -1;
            }

            pargvname = clish_parg__get_name(arg);
            if (!pargvname) {
                fprintf(stderr, "argv[%d] name is null\n", i);
                return -1;
            }

            if (!strcmp(pargvname, "reg-address"))
            {
                pargvregvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvregvalue);
            }
            else if (!strcmp(pargvname, "hex-value"))
            {
                pargvhexvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvhexvalue);
            }
            else
            {
                printf("Error: command %s parameter %s is invalid.\n", cmdname, pargvname);
                return -1;
            }

        }
        usRegOffset = (unsigned short)htoi(pargvregvalue);
        hexvalue = (unsigned)htoi(pargvhexvalue);

        sprintf(send_buf, regwrite_pattern, cmd_code, usRegOffset, hexvalue);
    }
    else if(strcmp (cmdname, "ramread") == 0)
    {
        const char *pargvramvalue;
        cmd_code = MSG_CLI_LMK05028_SRAM_READ;

        argc = clish_pargv__get_count(argv);
        for (i = 0; i < argc; i++) {
            arg = clish_pargv__get_parg(argv, i);
            if (!arg) {
                fprintf(stderr, "argv[%d] is null\n", i);
                return -1;
            }

            pargvname = clish_parg__get_name(arg);
            if (!pargvname) {
                fprintf(stderr, "argv[%d] name is null\n", i);
                return -1;
            }

            if (!strcmp(pargvname, "ram-address"))
            {
                pargvramvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvramvalue);
            }
            else
            {
                printf("Error: command %s parameter %s is invalid, should be %s.\n", cmdname, pargvname, "ram-address");
                return -1;
            }

        }
        usSramOffset = (unsigned short)htoi(pargvramvalue);
        
        sprintf(send_buf, ramread_pattern, cmd_code, usSramOffset);
    }
    else if(strcmp (cmdname, "romread") == 0)
    {
        const char *pargvromvalue;
        cmd_code = MSG_CLI_LMK05028_EEPROM_READ;

        argc = clish_pargv__get_count(argv);
        for (i = 0; i < argc; i++) {
            arg = clish_pargv__get_parg(argv, i);
            if (!arg) {
                fprintf(stderr, "argv[%d] is null\n", i);
                return -1;
            }

            pargvname = clish_parg__get_name(arg);
            if (!pargvname) {
                fprintf(stderr, "argv[%d] name is null\n", i);
                return -1;
            }

            if (!strcmp(pargvname, "rom-address"))
            {
                pargvromvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvromvalue);
            }
            else
            {
                printf("Error: command %s parameter %s is invalid, should be %s.\n", cmdname, pargvname, "rom-address");
                return -1;
            }

        }
        usEepromOffset = (unsigned short)htoi(pargvromvalue);
        
        sprintf(send_buf, romread_pattern, cmd_code, usEepromOffset);
    }
    else if (strcmp (cmdname, "cfgload") == 0)
    {
        const char *pargvpathvalue;
        cmd_code = MSG_CLI_LMK05028_CFG_LOAD;

        argc = clish_pargv__get_count(argv);
        for (i = 0; i < argc; i++) {
            arg = clish_pargv__get_parg(argv, i);
            if (!arg) {
                fprintf(stderr, "argv[%d] is null\n", i);
                return -1;
            }

            pargvname = clish_parg__get_name(arg);
            if (!pargvname) {
                fprintf(stderr, "argv[%d] name is null\n", i);
                return -1;
            }

            if (!strcmp(pargvname, "cfg-file"))
            {
                pargvpathvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvpathvalue);
            }
            else
            {
                printf("Error: command %s parameter %s is invalid, should be %s.\n", cmdname, pargvname, "cfg-file");
                return -1;
            }

        }
        sprintf(send_buf, cfgload_pattern, cmd_code, pargvpathvalue);
    }
    else if (strcmp (cmdname, "fsm") == 0)
    {
        const char *pargvfsmopvalue;
        cmd_code = MSG_CLI_LMK05028_FSM_OP;

        argc = clish_pargv__get_count(argv);
        for (i = 0; i < argc; i++) {
            arg = clish_pargv__get_parg(argv, i);
            if (!arg) {
                fprintf(stderr, "argv[%d] is null\n", i);
                return -1;
            }

            pargvname = clish_parg__get_name(arg);
            if (!pargvname) {
                fprintf(stderr, "argv[%d] name is null\n", i);
                return -1;
            }

            if (!strcmp(pargvname, "fsm-op"))
            {
                pargvfsmopvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvfsmopvalue);
            }
            else
            {
                printf("Error: command %s parameter %s is invalid, should be %s.\n", cmdname, pargvname, "fsm-op");
                return -1;
            }

        }

        if ( (!strcmp(pargvfsmopvalue, "up")) && (!strcmp(pargvfsmopvalue, "down")) )
        {
            printf("fsm up/down\n");  
            return -1;
        }  
        sprintf(send_buf, fsm_pattern, cmd_code, pargvfsmopvalue);
    }
    else if (strcmp (cmdname, "getstatus") == 0)
    {
        cmd_code = MSG_CLI_GET_TIMING_STATUS;
        sprintf(send_buf, status_pattern, cmd_code);
    }
    if(strcmp (cmdname, "zenqread") == 0)
    {
        const char *pargvregvalue;
        unsigned long ulRegOffset = 0;
        cmd_code = MSG_CLI_ZENQ_REG_READ;

        argc = clish_pargv__get_count(argv);
        for (i = 0; i < argc; i++) {
            arg = clish_pargv__get_parg(argv, i);
            if (!arg) {
                fprintf(stderr, "argv[%d] is null\n", i);
                return -1;
            }

            pargvname = clish_parg__get_name(arg);
            if (!pargvname) {
                fprintf(stderr, "argv[%d] name is null\n", i);
                return -1;
            }

            if (!strcmp(pargvname, "reg-address"))
            {
                pargvregvalue = clish_parg__get_value(arg);
                //printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvregvalue);
            }
            else
            {
                printf("Error: command %s parameter %s is invalid, should be %s.\n", cmdname, pargvname, "reg-address");
                return -1;
            }

        }
        ulRegOffset = (unsigned long)htoi(pargvregvalue);
        sprintf(send_buf, zenqread_pattern, cmd_code, ulRegOffset);
    }
    clish_handle_cmd(send_buf);

    return rc;
}


