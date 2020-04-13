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

bool gCLidExit = false;
bool gRouterExit = false;

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

unsigned long TimingMsg_Work_Task_Init(void)
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

static void Timing_Sig_Handler(int signal_value)
{
    static bool Sig_Flag = false;
    
    log_info("Timing catch SIGINT or SIGTERM, signal = %d", signal_value);
    if(!Sig_Flag){
        	Sig_Flag = true;
			gCLidExit = true;
			gRouterExit = true;
			pthread_cancel(ORUCli_Tid);
			pthread_cancel(ORURouter_Tid);
            ORUCli_Work_Task_Fini();
        	log_info("Timing exit");
        	exit(-1);
    	}
}

int Timing_Clish_Task_Entry()
{
	zmsg_t *Cmd_Msg;
	zframe_t *O_Identity;
	zframe_t *O_Content;
	size_t Frame_Len;
	char *Frame_Data = NULL;
	char Cmd_Str[256];
	log_info("Timing_Clish_Task_Entry");
	zpoller_t *poller = zpoller_new(O_Sock.CliToClish_Sync_Sock, NULL);

	while(!gCLidExit){
			zsock_t *which = zpoller_wait(poller, 10);
			if(!which){
		    		if (zpoller_expired(poller)) {
                		continue;
            		}
            		if (zpoller_terminated(poller)) {                
               	 		log_err("Interruptted or parent context was destroyed.");
                		break;
           			}			
				}
			else{
					Cmd_Msg = zmsg_recv(O_Sock.CliToClish_Sync_Sock);
					if(NULL != Cmd_Msg){
							O_Identity = zmsg_pop(Cmd_Msg);
							CLI_ASSERT(O_Identity);
							O_Content = zmsg_pop(Cmd_Msg);
							CLI_ASSERT(O_Identity);
							Frame_Len = zframe_size(O_Content);
							Frame_Data = zframe_data(O_Content);
							memcpy(Cmd_Str, Frame_Data, Frame_Len);
							log_info("CliToClish_Sync_Sock sync recv :%s\n",Cmd_Str);
                            timing_socket_rec(O_Identity, Cmd_Str);
							zframe_destroy(&O_Identity);
							zframe_destroy(&O_Content);
							zmsg_destroy(&Cmd_Msg);
							
						}	
					memset(Cmd_Str, 0, sizeof(Cmd_Str));
				}
		}

	zpoller_destroy(&poller);
	log_info("Timing_Clish_Task_Entry exit");
	return 0;	
}

int Timing_Router_Task_Entry()
{
	zmsg_t *Cmd_Msg;
	zframe_t *O_Identity;
	zframe_t *O_Content;
	size_t Frame_Len;
	char *Frame_Data = NULL;
	char Cmd_Str[256];
	log_info("Timing_Router_Task_Entry");
	zpoller_t *poller = zpoller_new(O_Sock.Router_Aync_Sock, NULL);

	while(!gRouterExit){
			zsock_t *which = zpoller_wait(poller, 10);
			if(!which){
		    		if (zpoller_expired(poller)) {
                		continue;
            		}
            		if (zpoller_terminated(poller)) {                
               	 		log_err("Interruptted or parent context was destroyed.");
                		break;
           			}			
				}
			else{
					Cmd_Msg = zmsg_recv(O_Sock.Router_Aync_Sock);
					if(NULL != Cmd_Msg){
							O_Identity = zmsg_pop(Cmd_Msg);
							CLI_ASSERT(O_Identity);
							O_Content = zmsg_pop(Cmd_Msg);
							CLI_ASSERT(O_Identity);
							Frame_Len = zframe_size(O_Content);
							Frame_Data = zframe_data(O_Content);
							memcpy(Cmd_Str, Frame_Data, Frame_Len);
							log_info("Router_Aync_Sock recv :%s\n",Cmd_Str);
							zframe_destroy(&O_Identity);
							zframe_destroy(&O_Content);
							zmsg_destroy(&Cmd_Msg);
							
						}	
					memset(Cmd_Str, 0, sizeof(Cmd_Str));
				}
		}

	zpoller_destroy(&poller);
	log_info("Timing_Router_Task_Entry exit");
	return 0;	
}

int Timing_Msg_Task_Start()
{
	int rc;
	if(-1 == TimingMsg_Work_Task_Init()){
			log_err("%s[%d], oru cli work task init failed", __FUNCTION__, __LINE__);
			return -1;
		}

    /* clish sync czmq sock */
	rc = pthread_create(&ORUCli_Tid, NULL,(void *)Timing_Clish_Task_Entry, NULL);
	if (rc) {
			log_err("%s[%d], create Timing_Clish_Task_Entry pthread failed", __FUNCTION__, __LINE__);
			return -1;
		}

    /* router async czmq sock */
	rc = pthread_create(&ORURouter_Tid, NULL,(void *)Timing_Router_Task_Entry, NULL);
	if (rc) {
			log_err("%s[%d], create Timing_Router_Task_Entry pthread failed", __FUNCTION__, __LINE__);
			return -1;
		}
	log_info("Timing_Msg_Task_Start");
	return 0;
}

unsigned long timing_msg_init(void)
{
    int rc;
    //set private signal handler
    zsys_handler_set(Timing_Sig_Handler);

    Timing_Msg_Task_Start();

    log_info("Timing message init success.");
    return 0;
}
