#include<stdio.h>
#include<string.h>

int main()
{
    char buf[50] = {0};
    FILE *out = fopen("/tmp/haha.txt", "w");

    for (int i = 0; i<500000; i++)
    {
        sprintf(buf, "This is a test line, it does not matter, %d\n", i);
        fwrite(buf, strlen(buf), 1, out);
    }
    //fseek(out, 0, SEEK_SET);
    rewind(out);

    sprintf(buf, "haha, it should be show this line at the begin\n");
    fwrite(buf, strlen(buf), 1, out);

    fclose(out);
    return 0;
}
