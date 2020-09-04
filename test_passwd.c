#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: %s username old new\n", argv[0]);
        return 1;
    }
    char cmd[32];
    sprintf(cmd, "su - %s -c passwd", argv[1]);
    FILE *fp= popen(cmd, "w");
    fprintf(fp, "%s\n", argv[2]);
    fprintf(fp, "%s\n", argv[3]);
    fprintf(fp, "%s\n", argv[3]);
    printf("Return value: %i\n", WEXITSTATUS(pclose(fp)));
    return 0;
}
