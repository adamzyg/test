#define _XOPEN_SOURCE 700
#include <stdio.h>  
#include <string.h>  
#include <time.h>  
#include <stdlib.h>    
int main(int argc, const char * argv[])  
{  
    struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));  
    strptime("2020-01-01 08:00:20","%Y-%m-%d %H:%M:%S",tmp_time); //时间24时制
    time_t t = mktime(tmp_time);  
    printf("%ld\n",t);  
    free(tmp_time);  

    char *test = strdup("haha");
    printf("test = %s\n", test);
    free(test);
    return 0;  
}  
