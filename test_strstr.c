#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[])
{
    char *res = strstr("xxxhost: www.baidu.com", "host");
    if(res == NULL) printf("res1 is NULL!\n");
    else printf("%s\n", res);    // print:-->'host: www.baidu.com'
    res = strstr("xxxhost: www.baidu.com", "cookie");
    if(res == NULL) printf("res2 is NULL!\n");
    else printf("%s\n", res);    // print:-->'res2 is NULL!'
    res = strstr("AV27-F470-P4CXP-FM", "AV27-F470-P4CXP");
    if(res == NULL) printf("res3 is NULL!\n");
    else printf("%s\n", res);    // print:-->'res2 is NULL!'
    return 0;
}
