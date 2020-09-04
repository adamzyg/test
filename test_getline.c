#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int main()
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *file = fopen("/home/zhengyagang/test/README.md", "r");

    while ((read = getline(&line, &len, file)) != -1)
    {
        printf("Read: %s", line);
        if (line == NULL)
        {
            free(line);
            line = NULL;
        }
    }
    fclose(file);
    return 0;
}
