#include<stdio.h>
#include<time.h>
#include<sys/time.h>

int main()
{
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmbuf[64], buf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    //nowtm = localtime(&nowtime);  //本地时间
    nowtm = gmtime(&nowtime);       //UTC时间
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
    snprintf(buf, sizeof(buf), "%s.%06ld", tmbuf, tv.tv_usec);
    printf("date = %s\n", buf);
    return 0;
}
