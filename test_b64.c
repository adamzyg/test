#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <b64/cdecode.h>
#include <b64/cencode.h>

int main()
{
    typedef struct {
        char a[10];
        int  b;
        char c[10];
        short d;
    } mystruct;

    mystruct ms = {
        .a = "hello", .b = 10, .c = "world", .d = 20
    };

    char *output = (char*)malloc(100);
    char *c = output;
    int cnt = 0;

    char *input = (char *)&ms;

    base64_encodestate s;
    base64_init_encodestate(&s);
    cnt = base64_encode_block(input, sizeof(mystruct), c, &s);
    c += cnt;
    *c = 0;
    printf("after base64 encode: %s\n", output);


    char* output2 = (char*)malloc(100);
    char *d = output2;
    base64_decodestate t;
    base64_init_decodestate(&t);
    cnt = base64_decode_block(output, strlen(output), d, &t);
    mystruct *pms = (mystruct*)(d); 
    printf("pms->a = %s\n", pms->a);
    printf("pms->b = %d\n", pms->b);
    printf("pms->c = %s\n", pms->c);
    printf("pms->d = %d\n", pms->d);


    free(output);
    free(output2);
    return 0;
}
