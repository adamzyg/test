#define _XOPEN_SOURCE 700
#include <stdio.h>  
#include <string.h>  
#include <time.h>  
#include <stdlib.h>    
int main(int argc, const char * argv[])  
{  
    struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));  
    //strptime("2000-01-01 00:30:00 UTC","%Y-%m-%d %H:%M:%S %z",tmp_time); //时间24时制
    strptime("2000-01-01 00:30:00 UTC","%Y-%m-%d %H:%M:%S %z",tmp_time); //时间24时制
    //tmp_time->tm_isdst = 0;
    time_t t = mktime(tmp_time);  
    printf("%ld\n",t);  
    free(tmp_time);  

    char *date = "2000-01-01T00:30:00Z";
    struct tm* tmp_time1 = (struct tm*)malloc(sizeof(struct tm));  
    strptime(date,"%FT%T%z",tmp_time1); //时间24时制
    time_t secs = mktime(tmp_time1) - timezone;
    printf("%ld\n",secs);  
    free(tmp_time1);  

    return 0;  
}  

