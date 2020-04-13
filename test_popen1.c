#include<stdio.h>
#include<string.h>

int main()
{
    char cmdStr[100];
    sprintf(cmdStr, "timeout 15s sleep 20");
    FILE *Fpr = popen(cmdStr, "r");
    char buf[128];
    memset(buf, 0, 128);
    while (fgets(buf, 128, Fpr) != NULL)
    {
        printf("%s", buf);
    }
    pclose(Fpr);
    return 0;
}

