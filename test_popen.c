#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int main()
{
    char cmdStr[100];
    FILE *Fpr;
    char buf[128];
    memset(buf, 0, 128);
    Fpr = popen("pidof a.out", "r");
    fgets(buf, 10, Fpr);
    pid_t pid = strtoul(buf, NULL, 10);
    pclose(Fpr);
    printf("pidof a.out = %d\n", pid);

    sprintf(cmdStr, "less /var/log/dpkg.log");
    Fpr = popen(cmdStr, "w");
    //memset(buf, 0, 128);
    //while (fgets(buf, 128, Fpr) != NULL)
    //{
    //    printf("%s", buf);
    //}
    pclose(Fpr);


    return 0;
}

