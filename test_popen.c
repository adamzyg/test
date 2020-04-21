#include<stdio.h>
#include<string.h>

int main()
{
    char cmdStr[100];
    FILE *Fpr;
    char buf[128];
    memset(buf, 0, 128);
    sprintf(cmdStr, "less /var/log/dpkg.log");
    Fpr = popen(cmdStr, "w");
    memset(buf, 0, 128);
    while (fgets(buf, 128, Fpr) != NULL)
    {
        printf("%s", buf);
    }
    pclose(Fpr);
    return 0;
}

