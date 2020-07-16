#include <stdio.h>

int func()
{
    char test[10] = {0};
    static char *p = "first";
    static int a = 5;
    a++;
    printf("current static int a = %d, statis char *p = %s\n", a, p);
    sprintf(test, "%d", a);
    p = test;
    return 0;
}


int main()
{
    func();
    func();
    func();
    return 0;
}
