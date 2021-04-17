#include <stdio.h>
#include <stdlib.h>
int main(){
    char *a = "-100.23",
         *b = "200e-2",
         *c = "341",
         *d = "100.34cyuyan",
         *e = "cyuyan";
    printf("a = %.2f\n", atof(a));
    printf("b = %.2f\n", atof(b));
    printf("c = %.2f\n", atof(c));
    printf("d = %.2f\n", atof(d));
    printf("e = %.2f\n", atof(e));
    system("pause");
    return 0;
}
