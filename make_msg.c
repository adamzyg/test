#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

char * make_message(const char *fmt, va_list argptr)
{
    int size = 0;
    char *p = NULL;
    va_list ap;

    /* Determine required size */

    va_copy(ap, argptr);
    size = vsnprintf(p, size, fmt, ap);

    if (size < 0)
        return NULL;

    size++;             /* For '\0' */
    p = malloc(size);
    if (p == NULL)
        return NULL;

    va_copy(ap, argptr);
    size = vsnprintf(p, size, fmt, ap);
    if (size < 0) {
        free(p);
        return NULL;
    }
    va_end(ap);

    return p;
}

int std_print(const char *fmt, ...)
{
    va_list argptr;
    
    va_start (argptr, fmt);
    char *string = make_message(fmt, argptr);
    va_end (argptr);

    fprintf(stdout, "%s\n", string);
    if (string)
        free(string);    
}

int main()
{
    std_print("test 1 %d, %s \n", 10, "haha");
    unsigned char x = 0x33;
    printf("%x\n", (x << 8) & 0x1f00);
    printf("%x\n", x);
    return 0;
}

