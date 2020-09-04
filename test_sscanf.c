#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int match(char *buf, char *pattern, char *substr1, char *substr2)
{
    regmatch_t pmatch[3];
    size_t nmatch = 3;
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
		if(pmatch[1].rm_so >= 0)
		{
			memcpy(substr1, buf+pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
		}
		if(pmatch[2].rm_so >= 0)
		{
			memcpy(substr2, buf+pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
		}
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
    char *origin = "1,4,192.168.1.10:1234";
    //char *format = "1,4,\\([0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}\\)";
    char *format = "^1,4,\\([0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\)\\(:[0-9]\\+\\)\\?";
    char str[2][16] = {{0}};
    char *ipaddr = NULL;
    char *port = NULL;

    int rc = match(origin, format, str[0], str[1]);
    printf("found %s and %s return %d\n", str[0], str[1], rc);
    if (rc == 0)
    {
        ipaddr = str[0];
        if (str[1][0] == 0)
        {
            port = "4334";
        }
        else
        {
            port = &str[1][1];
        }
    }
    printf("Found ipaddr %s port %s\n", ipaddr, port);
    return 0;
}
