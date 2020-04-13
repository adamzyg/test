#include<stdio.h>
#include<string.h>

int main()
{
    char cmdStr[100];
    sprintf(cmdStr, "ping 127.0.0.1 -c 3");
    FILE *Fpr = popen(cmdStr, "r");
    char *ping_loss_pattern = "0\% packet loss";
    short ping_flag = 1;
    char buf[128];
    memset(buf, 0, 128);
    while (fgets(buf, 128, Fpr) != NULL)
    {
        printf("%s", buf);
        if (strstr(buf, ping_loss_pattern))
        {
            printf("Found!!!!!!!!!!!!!!!!!\n");
        }
    }
    pclose(Fpr);

    sprintf(buf, "3 packets transmitted, 3 received, 0%% packet loss, time 2033ms\n");
    printf("%s", buf);
    if (strstr(buf, ping_loss_pattern))
    {
        printf("Found!!!!!!!!!!!!!!!!!\n");
    }

    printf("%s\n", cmdStr);
    return 0;
}


