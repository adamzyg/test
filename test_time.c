#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(void)
{
    long i = 1000000000L;
    clock_t start, finish;
    int duration;
    /* 测量一个事件持续的时间*/
    printf( "Time to do %ld empty loops is ", i) ;
    start = clock();
    while( i-- );
    finish = clock();
    duration = (finish - start) / CLOCKS_PER_SEC;
    printf( "%d seconds\n", duration );
    return 0;
}
