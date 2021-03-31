#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main()
{
    char prefix = 0;
    unsigned long mask = (0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF;

    printf("%lu.%lu.%lu.%lu\n", mask >> 24, (mask >> 16) & 0xFF, (mask >> 8) & 0xFF, mask & 0xFF);
    return 0;
}

