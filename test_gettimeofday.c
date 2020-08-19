#include <stdio.h>        // for printf()
#include <sys/time.h>    // for gettimeofday()
#include <unistd.h>        // for sleep()

int main()
{
    struct timeval start, end;
    struct timezone start_tz, end_tz;
    gettimeofday( &start, &start_tz );
    printf("start : %d.%d %ld%ld\n", start.tv_sec, start.tv_usec, start_tz.tz_minuteswest, start_tz.tz_dsttime);
    sleep(1);
    gettimeofday( &end, &end_tz);
    printf("end   : %d.%d\n", end.tv_sec, end.tv_usec);

    return 0;
}
