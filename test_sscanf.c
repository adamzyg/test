#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int match(char *buf, char *pattern, char *match_str)
{
    regmatch_t pmatch[2];
    size_t nmatch = 2;
    regex_t reg;
    int rc;

    if (0 != (rc = regcomp(&reg, pattern, 0)))
    {
        printf("regcomp() failed, returning nonzero (%d)\n", rc);
        return -1;
    }
    int status = regexec(&reg, buf, nmatch, pmatch, 0);
    if(status == REG_NOMATCH)
    {
        regfree(&reg);
        return -1;
    }
    else if(pmatch[1].rm_so != -1)
    {
        memcpy(match_str, buf+pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        regfree(&reg);
        return 0;
    }
    else
    {
        regfree(&reg);
        return -2;
    }
}

int main()
{
    char *origin = "/ietf-interfaces:interfaces/interface[name='eth0-abc_cde']/name";
    char *format = "/ietf-interfaces:interfaces/interface\\[name='\\([-_a-z0-9]\\+\\)'\\]/name";
    char str[16] = {0};

    int rc = match(origin, format, str);
    printf("found %s return %d\n", str, rc);
    return 0;
}
