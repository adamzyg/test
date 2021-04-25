#include<stdio.h>

int test(char *buf)
{
    if (buf[0] != 0)
    {
        printf("in test buf = %s\n", buf);
    }
    return 0;
}

int main()
{
    printf("in main\n");
    char aaa[32] = "abc";
    test(aaa);
end:
    printf("in end\n");
    return 0;
}
