#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    int a;
    char b[16];
} mystruct;

int main()
{
    mystruct *this = (mystruct *)malloc(sizeof(mystruct));
    printf("sizeof(b) == %ld\n", sizeof(this->b));
    free(this);
    return 0;
}
