#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

static int time_and_date(char *next_notif_time)
{
    time_t cur_time = 0;
    time_t next_time = 0;
    struct tm *ptm = NULL;

    time(&cur_time);
    struct tm lt = {0};
    ptm = localtime_r(&cur_time, &lt);
    sprintf(next_notif_time, "%d-%02d-%02dT%02d:%02d:%02d%+03d:%02d", ptm->tm_year + 1900,
            ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)lt.tm_gmtoff/3600, abs((int)lt.tm_gmtoff/60%60));
    return 0;
}
#if 0
char * nc_time2datetime(time_t time, const char *tz, char *buf)
{
    char *date = NULL;
    char *zoneshift = NULL;
    int zonediff, zonediff_h, zonediff_m;
    struct tm tm, *tm_ret;
    char *tz_origin;

    if (tz) {
        tz_origin = getenv("TZ");
        if (tz_origin) {
            tz_origin = strdup(tz_origin);
            if (!tz_origin) {
                ERRMEM;
                return NULL;
            }
        }
        setenv("TZ", tz, 1);
        tzset(); /* apply timezone change */
        tm_ret = localtime_r(&time, &tm);
        if (tz_origin) {
            setenv("TZ", tz_origin, 1);
            free(tz_origin);
        } else {
            unsetenv("TZ");
        }
        tzset(); /* apply timezone change */

        if (!tm_ret) {
            return NULL;
        }
    } else {
        if (!gmtime_r(&time, &tm)) {
            return NULL;
        }
    }

    /* years cannot be negative */
    if (tm.tm_year < -1900) {
        ERRARG("time");
        return NULL;
    }

    if (tm.tm_gmtoff == 0) {
        /* time is Zulu (UTC) */
        if (asprintf(&zoneshift, "Z") == -1) {
            ERRMEM;
            return NULL;
        }
    } else {
        zonediff = tm.tm_gmtoff;
        zonediff_h = zonediff / 60 / 60;
        zonediff_m = zonediff / 60 % 60;
        if (asprintf(&zoneshift, "%+03d:%02d", zonediff_h, zonediff_m) == -1) {
            ERRMEM;
            return NULL;
        }
    }

    if (buf) {
        sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d%s",
                        tm.tm_year + 1900,
                        tm.tm_mon + 1,
                        tm.tm_mday,
                        tm.tm_hour,
                        tm.tm_min,
                        tm.tm_sec,
                        zoneshift);
    } else if (asprintf(&date, "%04d-%02d-%02dT%02d:%02d:%02d%s",
                        tm.tm_year + 1900,
                        tm.tm_mon + 1,
                        tm.tm_mday,
                        tm.tm_hour,
                        tm.tm_min,
                        tm.tm_sec,
                        zoneshift) == -1) {
        free(zoneshift);
        ERRMEM;
        return NULL;
    }
    free(zoneshift);

    return (buf ? buf : date);
}
#endif
int main()
{
    char current_time[64] = {0};
    time_and_date(current_time);
    printf("current time: %s\n", current_time);
    return 0;
}
