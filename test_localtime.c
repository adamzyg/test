#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    time_t nowtime = 0;
    struct tm *nowtm;
    char tmbuf[64] = {0};
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
    printf("time: %s\n", tmbuf);
    return 0;
}
