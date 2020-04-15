#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

int htoi(const char s[])  
{  
    int i;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
    {  
        i = 2;  
    }  
    else  
    {  
        i = 0;  
    }  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
    {  
        if (tolower(s[i]) > '9')  
        {  
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        }  
        else  
        {  
            n = 16 * n + (tolower(s[i]) - '0');  
        }  
    }  
    return n;  
}  

int main()
{
    char cmdStr[100];
    char *ptr = NULL;
    char *hex = "0x12";
    int number = htoi(hex);
    printf("number: %d\n", number);
    char *test1 = "{\"test\": %d}";
    fprintf(stdout, test1, 10);

    fprintf(stdout, "test1\n");
    fprintf(stderr, "test2\n");
    sprintf(cmdStr, "i2cget -y %d 0x%x 0x%x",1, 0x6c, 0x0);
    printf("%s\n", cmdStr);
    sprintf(cmdStr, "0x0141\n");
    ptr = cmdStr + strlen(cmdStr) + 1;
    unsigned int test = strtol(cmdStr, &ptr, 16);
    printf("test = 0x%x\n", test);
    printf("test = %d\n", test);

    unsigned long lint = 0xfabc;
    printf("lint = 0x%lx\n", lint);
    return 0;
}

