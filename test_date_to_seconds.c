//#define _XOPEN_SOURCE 700
#include <stdio.h>  
#include <string.h>  
#include <time.h>  
#include <stdlib.h>    
int main(int argc, const char * argv[])  
{  
	char buf[255];
#if 0
    struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));  
    //strptime("2000-01-01 00:30:00 UTC","%Y-%m-%d %H:%M:%S %z",tmp_time); //时间24时制
    strptime("2000-01-01 00:30:00 +00:00","%Y-%m-%d %H:%M:%S %z",tmp_time); //时间24时制
    //tmp_time->tm_isdst = 0;
    time_t t = mktime(tmp_time);  
    printf("%ld\n",t);  
    free(tmp_time);  
#endif

#if 0
    time_t timestamp_from_datetime(const char *datetime)
    {
        struct tm time_struct = (struct tm) {0};
        strptime(datetime,"%Y%m%dT%H%M%S%z", &time_struct);
        long int gmtoff = time_struct.tm_gmtoff;
        return mktime(&time_struct) - gmtoff - timezone;
    }
#endif

    char *date = "2000-01-01T00:30:00Z";
    struct tm* tmp_time1 = (struct tm*)malloc(sizeof(struct tm));  
    strptime(date,"%FT%T%z",tmp_time1); //时间24时制
	strftime(buf, sizeof(buf), "%d %b %Y %H:%M %z", tmp_time1);
	puts(buf);
    printf("timezone = %ld\n", timezone);
    printf("tm.tm_gmtoff = %ld tm.tm_zone = %s\n", tmp_time1->tm_gmtoff, tmp_time1->tm_zone);
    time_t gmtoff = tmp_time1->tm_gmtoff;
    time_t secs1 = mktime(tmp_time1) - gmtoff - timezone;
    printf("secs1 = %ld\n", secs1);  
    free(tmp_time1);  

    char *date1 = "2000-01-01T00:30:00+08:00";
    struct tm* tmp_time2 = (struct tm*)malloc(sizeof(struct tm));  
    strptime(date1,"%FT%T%z",tmp_time2); //时间24时制
	strftime(buf, sizeof(buf), "%d %b %Y %H:%M %z", tmp_time2);
	puts(buf);
    printf("timezone = %ld\n", timezone);
    printf("tm_gmtoff = %ld tm.tm_zone = %s\n", tmp_time2->tm_gmtoff, tmp_time2->tm_zone);
    time_t gmtoff2 = tmp_time1->tm_gmtoff;
    time_t secs2 = mktime(tmp_time2) - gmtoff2 - timezone;
    printf("%ld\n",secs2);  
    free(tmp_time2);  

    return 0;  
}  

