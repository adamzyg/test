#include <stdio.h>

int func()
{
    static int a = 5;
    a++;
    printf("current static int a = %d\n", a);
    return 0;
}


int main()
{
    func();
    func();
    func();
    return 0;
}
