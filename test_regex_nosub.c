#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int match(char *buf, char *pattern)
{
    regmatch_t pmatch[1];
    size_t nmatch = 1;
    regex_t reg;
    int rc;

    if (0 != (rc = regcomp(&reg, pattern, 0)))
    {
        fprintf(stderr, "regcomp() failed, returning nonzero (%d)\n", rc);
        return -1;
    }
    int status = regexec(&reg, buf, nmatch, pmatch, 0);
    if(status == 0)
    {
        printf("buf %s match pattern %s\n", buf, pattern);
		regfree(&reg);
        return 0;
    }
    else
    {
        regfree(&reg);
        return 1;
    }
}

int main()
{
    char *str = "ABCDEFG";
    char *pattern = "*A*";
    char new_pattern[256] = {0};
    unsigned short len = strlen(pattern);
    int j = 0;
    for (int i = 0; i < len; i++)
    {
        if (pattern[i] == '*')
        {
            new_pattern[j++] = '.';
            new_pattern[j++] = '*';
        }
        else
        {
            new_pattern[j++] = pattern[i];
        }
    }
    new_pattern[j++] = '\0';
    printf("new_pattern: %s\n", new_pattern);
    return match(str, new_pattern);
}

