#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>

#include<cJSON.h>

#include "timing.h"
#include "timing_msg.h"
#include "timing_source.h"
#include "timing_chip.h"
#include "devm.h"

extern timing_state_t g_stTimingStates;
extern timing_cfg_t g_stTimingCfgInfo;

/*process communication FD*/
zloop_t *gTiming_loop = NULL;


#define TIMING_SOCKET_PORT              8003
#define IPC_MSG_MAX_BUF_SIZE            2000

#define FPD_APB_BASE                0xa0000000UL
#define FPD_APB_REG(reg)            (FPD_APB_BASE + (((uint32_t)((reg) & 0xffff)) << 2))


size_t strlcat(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;
    size_t dlen;

    assert(dst != NULL);
    assert(src != NULL);

    /* Find the end of dst and adjust bytes left but don't go past end */
    while (n-- != 0 && *d != '\0')
        d++;
    dlen = d - dst;
    n = siz - dlen;

    if (n == 0)
        return(dlen + strlen(s));
    while (*s != '\0') {
        if (n != 1) {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';

    return(dlen + (s - src));/* count does not include NUL */
}

int ORUCli_Resp_To_Clish(zframe_t *id, bool IsEnd, const char *fmt, ...)
{
    int rc;
    char buf[512];
    zmsg_t *msg;
    zframe_t *idf;

    va_list args;

    buf[0] = '\0';
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if(IsEnd)
        strlcat(buf, RepEnd, sizeof(buf));
    msg = zmsg_new();
    idf = zframe_dup(id);
    zmsg_append(msg, &idf);
    zmsg_addstr(msg, buf);
    rc = zmsg_send(&msg, O_Sock.CliToClish_Sync_Sock);

    if (rc) {
        log_err("%s[%d], cli to clish send message failed", __FUNCTION__, __LINE__);
        rc = -1;
    }

    return rc;
}

int timing_socket_rec(zframe_t *id, char *arg)
{
    unsigned long ulRet = 0;
    int iRet = 0;

    cJSON *json, *cmd;
    char *ret_pattern = NULL;

    uint32_t ulRegOffset = 0;
    unsigned long ulTemp = 0;

    unsigned short usRegOffset = 0;
    unsigned short usEepromOffset = 0;
    unsigned short usSramOffset = 0;
    unsigned char ucTemp = 0;

    unsigned short usPhyAddr = 0;
    unsigned short usDeviceId = 0;
    unsigned short usValue = 0;

    log_info ("come in socket rec test ...\n");

    json = cJSON_Parse(arg);
    if(NULL == json)
    {
        log_err("Error json format: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }

    cmd = cJSON_GetObjectItem(json, "cmd");
    if(cmd->type == cJSON_Number)
    {
        int cmd_code = cmd->valueint;

        switch (cmd_code)
        {
            case MSG_CLI_TIMING_REG_READ:
                // req json: {"cmd": 0x01, "address": 0x1}
                ret_pattern = "{\"status\": %d, \"return\": %d}";
                //log_info ("Reg offset [data0:0x%x data1:0x%x].\n", stMsgHeader->ucData[0],stMsgHeader->ucData[1]);
                usRegOffset = cJSON_GetObjectItem(json, "address")->valueint;

                log_info ("Reg offset [0x%x].\n", usRegOffset);
                ucTemp = 0;
                ulRet = timing_reg_read(usRegOffset, &ucTemp);
                if (ulRet != TIMING_OK)
                {
                    log_err("timing reg[0x%x] read fail.\n", usRegOffset);
                    ucTemp = 0xff;
                }
                else
                {
                    log_info ("Reg offset [0x%x] value [0x%x].\n", usRegOffset, ucTemp);
                }
                ORUCli_Resp_To_Clish(id, true, ret_pattern, ulRet, ucTemp);
                break;

            case MSG_CLI_TIMING_REG_WRITE:
                // req json: {"cmd": 0x02, "address": 0x1, "value": 0x02}
                ret_pattern = "{\"status\": %d}";
                usRegOffset = cJSON_GetObjectItem(json, "address")->valueint;
                ucTemp = cJSON_GetObjectItem(json, "value")->valueint;

                log_info ("Reg offset [0x%x] value [0x%x].\n", usRegOffset, ucTemp);
                ulRet = timing_reg_write(usRegOffset, ucTemp);
                if (ulRet != TIMING_OK)
                {
                    log_err("timing reg[0x%x] write fail.\n", usRegOffset);
                }
                ORUCli_Resp_To_Clish(id, true, ret_pattern, ulRet);
                break;

            case MSG_CLI_GET_TIMING_STATUS:
                // req json: {"cmd": 0x04}
                ret_pattern = "{\"status\": %d, \"FSM\": %d, \"CurrentSource\": %d, \"FefClockStates\": %d}";
                ORUCli_Resp_To_Clish(id, true, ret_pattern, TIMING_OK, g_stTimingStates.ucFsm, g_stTimingStates.ucCurrentSource, g_stTimingStates.ucFefClockStates[g_stTimingStates.ucCurrentSource]);
                break;

            case MSG_CLI_ZENQ_REG_READ:
                // req json: {"cmd": 0x05, "address": 0x1}
                ret_pattern = "{\"status\": %d, \"return\": %d}";
                ulTemp = 0;
                usRegOffset = cJSON_GetObjectItem(json, "address")->valueint;

                log_info ("Reg offset [0x%x].\n", ulRegOffset);
                //iRet = devm_readl(FPD_APB_REG(ulRegOffset), &ulTemp);
                iRet = fpga_reg_read(ulRegOffset, &ulTemp);
                if (iRet != TIMING_OK)
                {
                    log_err("fpga reg[0x%x] read fail.\n", ulRegOffset);
                    ulTemp = 0xffffffff;
                }
                else
                {
                    log_info ("fpga Reg offset [0x%x] value [0x%x].\n", ulRegOffset, ulTemp);
                }
                ORUCli_Resp_To_Clish(id, true, ret_pattern, ulRet, ucTemp);
                break;

            case MSG_CLI_ZENQ_REG_WRITE:
                break;

            case MSG_CLI_PHY_REG_READ:
                // req json: {"cmd": 0x07,  "phy": 0x1, "device": 0x1, "address": 0x1}
                ret_pattern = "{\"status\": %d, \"return\": %d}";
                usPhyAddr = cJSON_GetObjectItem(json, "phy")->valueint;

                usDeviceId = cJSON_GetObjectItem(json, "device")->valueint;

                usRegOffset = cJSON_GetObjectItem(json, "address")->valueint;

                log_info ("phy reg read:usPhyAddr[0x%x] usDeviceId[0x%x] usRegOffset[0x%x].\n", usPhyAddr, usDeviceId, usRegOffset);
                ulRet = phy_reg_read(usPhyAddr, usDeviceId, usRegOffset, &usValue);
                if (ulRet != TIMING_OK)
                {
                    log_err("phy reg read fail.\n");
                    usValue = 0xffff;
                }
                else
                {
                    log_info ("phy read value [0x%x].\n", usValue);
                }
                ORUCli_Resp_To_Clish(id, true, ret_pattern, ulRet, ucTemp);

                break;

            case MSG_CLI_PHY_REG_WRITE:
                // req json: {"cmd": 0x07,  "phy": 0x1, "device": 0x1, "address": 0x1, "value": 0x2}
                ret_pattern = "{\"status\": %d}";
                usPhyAddr = cJSON_GetObjectItem(json, "phy")->valueint;

                usDeviceId = cJSON_GetObjectItem(json, "device")->valueint;

                usRegOffset = cJSON_GetObjectItem(json, "address")->valueint;

                usValue += cJSON_GetObjectItem(json, "value")->valueint;

                log_info ("phy reg write:usPhyAddr[0x%x] usDeviceId[0x%x] usRegOffset[0x%x] usValue[0x%x].\n", usPhyAddr, usDeviceId, usRegOffset, usValue);
                ulRet = phy_reg_write(usPhyAddr, usDeviceId, usRegOffset, usValue);
                if (ulRet != TIMING_OK)
                {
                    log_err("phy reg write fail.\n");
                }
                ORUCli_Resp_To_Clish(id, true, ret_pattern, ulRet);
                break;

            case MSG_CLI_TIMING_FSM_OP:
                // req json: {"cmd": 0x0f, "op": "up"}
                ret_pattern = "{\"status\": %d}";
                char *op = cJSON_GetObjectItem(json, "op")->valuestring;
                ulRet = timing_fsm_op(op);
                if (ulRet != TIMING_OK)
                {
                    log_err("timing stop FSM fail.\n");
                }
                ORUCli_Resp_To_Clish(id, true, ret_pattern, ulRet);
                break;

            default:
                log_err ("MSGID[0x%x] isn't exist.\n", cmd_code);
                ret_pattern = "{\"status\": %d, \"reason\": \"%s\"}";
                ORUCli_Resp_To_Clish(id, true, ret_pattern, -1, "unsupport command");
                break;

        }
    }
    if (json)
    {
        cJSON_Delete(json);
        json = NULL;
    }
    return 0;

}

void ORU_Cli_Load_Syscfg()
{
    //char *SysCfg_Data = NULL;
    //zconfig_t *root = NULL;

    memset(&O_SysCfg, 0, sizeof(O_SysCfg));

    O_SysCfg.CliToClish_sync_sock_port = 8003;
    O_SysCfg.Router_aync_sock_port = 8004;
    //O_SysCfg.CliToPtp_sync_sock_port = 10703;
}

void ORUCli_Work_Task_Fini()
{
    log_info("%s[%d], destory timing czmq sock now!", __FUNCTION__, __LINE__);
    if(O_Sock.CliToClish_Sync_Sock){
        zsock_destroy(&O_Sock.CliToClish_Sync_Sock);
    }
    if(O_Sock.Router_Aync_Sock)
    {
        zsock_destroy(&O_Sock.Router_Aync_Sock);
    }
}

unsigned long ORUCli_Work_Task_Init(void)
{
    int rc;
    log_info("%s[%d], ORU cli work task init now!", __FUNCTION__, __LINE__);

    ORU_Cli_Load_Syscfg();
    /**
      create and bind cli to clish sock
     **/
    O_Sock.CliToClish_Sync_Sock = zsock_new(ZMQ_ROUTER);
    if (NULL == O_Sock.CliToClish_Sync_Sock){
        log_err("%s[%d], create ORU clish asyn socket failed", __FUNCTION__, __LINE__);
        rc = -1;
        goto end;
    }
    rc = zsock_bind(O_Sock.CliToClish_Sync_Sock, "tcp://*:%d", O_SysCfg.CliToClish_sync_sock_port);
    if (-1 == rc){
        log_err("%s[%d], bind cli to clish asyn sockt sock failed", __FUNCTION__, __LINE__);
        rc = -1;
        goto end;
    }
    log_info("bind router port: %d\n", O_SysCfg.CliToClish_sync_sock_port);

    /**
      create timing asyn event sock
     **/
    O_Sock.Router_Aync_Sock = zsock_new(ZMQ_ROUTER);
    if (NULL == O_Sock.Router_Aync_Sock){
        log_err("%s[%d], create ORU event asyn sockt sock failed", __FUNCTION__, __LINE__);
        rc = -1;
        goto end;
    }
    zsock_bind(O_Sock.Router_Aync_Sock, "tcp://*:%d", O_SysCfg.Router_aync_sock_port);
    if(NULL == O_Sock.Router_Aync_Sock)
    {
        log_err("bind handle route sync sock to port[%d] failed", O_SysCfg.Router_aync_sock_port);
        rc = -1;
        goto end;
    }

end:
    if(-1 == rc){
        ORUCli_Work_Task_Fini();
    }
    return rc;
}

static void Timing_signal_handler (int signal_value)
{
    static bool flag = false;

    log_info("timing catch SIGINT or SIGTERM, signal = %d", signal_value);

    if(!flag)
    {
        flag = true;
        //destory work tasks
        ORUCli_Work_Task_Fini();
        zloop_destroy (&gTiming_loop);
        log_info("Timing exit by signal.\n");
        exit(-1);
    }
}

static int Timing_handle_SyncMsg (zloop_t *loop, zsock_t *handle, void *arg)
{
    zmsg_t *Cmd_Msg;
    zframe_t *O_Identity;
    zframe_t *O_Content;
    size_t Frame_Len;
    char *Frame_Data = NULL;
    char Cmd_Str[256];

    log_info("recv sync msg in %s\n", __FUNCTION__);
    Cmd_Msg = zmsg_recv(handle);
    memset(Cmd_Str, 0, sizeof(Cmd_Str));

    if(NULL != Cmd_Msg){
        O_Identity = zmsg_pop(Cmd_Msg);
        CLI_ASSERT(O_Identity);
        O_Content = zmsg_pop(Cmd_Msg);
        CLI_ASSERT(O_Content);
        Frame_Len = zframe_size(O_Content);
        Frame_Data = zframe_data(O_Content);
        memcpy(Cmd_Str, Frame_Data, Frame_Len);
        log_info("recv sync event:%s\n",Cmd_Str);
        timing_socket_rec(O_Identity, Cmd_Str);
        zframe_destroy(&O_Identity);
        zframe_destroy(&O_Content);
        zmsg_destroy(&Cmd_Msg);
    }	

    return 0;
}

static int Timing_handle_AsyncMsg (zloop_t *loop, zsock_t *handle, void *arg)
{
    zmsg_t *Cmd_Msg;
    zframe_t *O_Identity;
    zframe_t *O_Content;
    size_t Frame_Len;
    char *Frame_Data = NULL;
    char Cmd_Str[256];

    log_info("recv async msg in %s\n", __FUNCTION__);
    Cmd_Msg = zmsg_recv(handle);
    memset(Cmd_Str, 0, sizeof(Cmd_Str));

    if(NULL != Cmd_Msg){
        O_Identity = zmsg_pop(Cmd_Msg);
        CLI_ASSERT(O_Identity);
        O_Content = zmsg_pop(Cmd_Msg);
        CLI_ASSERT(O_Content);
        Frame_Len = zframe_size(O_Content);
        Frame_Data = zframe_data(O_Content);
        memcpy(Cmd_Str, Frame_Data, Frame_Len);
        log_info("recv asyn event:%s\n",Cmd_Str);
        //timing_socket_rec(O_Identity, Cmd_Str);
        zframe_destroy(&O_Identity);
        zframe_destroy(&O_Content);
        zmsg_destroy(&Cmd_Msg);
    }	

    return 0;
}

void Timing_register_asyn_callback()
{
    //register reader callback
    zloop_reader (gTiming_loop, O_Sock.CliToClish_Sync_Sock, Timing_handle_SyncMsg, NULL);
    zloop_reader (gTiming_loop, O_Sock.Router_Aync_Sock, Timing_handle_AsyncMsg, NULL);

    zloop_reader_set_tolerant (gTiming_loop, O_Sock.CliToClish_Sync_Sock);
    zloop_reader_set_tolerant (gTiming_loop, O_Sock.Router_Aync_Sock);
}

unsigned long timing_msg_init(void)
{
    int rc;
    zsys_init();
    gTiming_loop = zloop_new();
    //set private signal handler
    zsys_handler_set(Timing_signal_handler);

    ORUCli_Work_Task_Init();
    //register rx msg callback
    Timing_register_asyn_callback();
    log_info("OltCli_Work_Task_Start");


    //start zloop
    zloop_start (gTiming_loop);

    //ORUCli_Work_Task_Fini();

    //zloop_destroy (&gTiming_loop);
    log_info("Timing init success.");
    return 0;
}
