#include <stdio.h>
#include <stdlib.h>

typedef enum ageingstatus
{
    AG_INIT = 0,
    AG_ONGOING,
    AG_FAIL,
    AG_SUCCESS,
} EN_AG_STATUS;

int main()
{
   char str[30] = "2030300";
   char *ptr;
   char intstr[4] = "1";
   long ret;
   EN_AG_STATUS er;
   EN_AG_STATUS *enResult = &er;

   ret = strtol(str, &ptr, 10);
   printf("数字（无符号长整数）是 %ld\n", ret);
   printf("字符串部分是 |%s|", ptr);
   *enResult = atoi(intstr);
   printf("enResult: %d\n", *enResult);

   return(0);
}

