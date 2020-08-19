#define _GNU_SOURCE /* for tm_gmtoff and tm_zone */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Checking errors returned by system calls was omitted for the sake of readability. */
int main(void)
{
  time_t t = time(NULL);
  struct tm lt = {0};

  localtime_r(&t, &lt);

  printf("Offset to GMT is %lds.\n", lt.tm_gmtoff);
  printf("The time zone is '%s'.\n", lt.tm_zone);

  printf("number1: %+03d:%02d\n", 27800/3600, abs((27800/60)%60));
  printf("number2: %+03d:%02d\n", -27800/3600, abs((-27800/60)%60));

  return 0;
}
