#include<stdio.h>

#define STATUS_UNLOCKED     0
#define STATUS_FREQLOCKED   1
#define STATUS_PHASELOCKED  2
#define STATUS_SERVOLOCKED  3
#define STATUS_TIMELOCKED   4

char *servo_status_switch(int servo_status)
{
    if (servo_status == STATUS_UNLOCKED) {
        return "STATUS_UNLOCKED";
    }
    else if (servo_status == STATUS_FREQLOCKED) {
        return "STATUS_FREQLOCKED";
    }
    else if (servo_status == STATUS_PHASELOCKED) {
        return "STATUS_PHASELOCKED";
    }
    else if (servo_status == STATUS_SERVOLOCKED) {
        return "STATUS_SERVOLOCKED";
    }
    else if (servo_status == STATUS_TIMELOCKED) {
        return "STATUS_TIMELOCKED";
    }
    else 
    {
        return "STATUS_UNKNOW";
    }
}

int main()
{
    for(int i=0; i<6; i++)
    {
        printf("i = %d, status = %s\n", i, servo_status_switch(i));
    }
    return 0;
}
