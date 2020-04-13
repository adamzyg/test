#include<stdio.h>

void func(unsigned char * c)
{
    *c = 0x40;
}

int main()
{
    unsigned char a = 0xFF;
    printf("haha %c\n", a);
    func(&a);
    printf("haha %c\n", a);
    return 0;
}

