#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int match(char *buf, char *pattern, char *substr1, char *substr2, char *substr3, char *substr4)
{
    regmatch_t pmatch[5];
    size_t nmatch = 5;
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
            substr1[pmatch[1].rm_eo - pmatch[1].rm_so] = '\0';
		}
		if(pmatch[2].rm_so >= 0)
		{
			memcpy(substr2, buf+pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
            substr2[pmatch[2].rm_eo - pmatch[2].rm_so] = '\0';
		}
		if(pmatch[3].rm_so >= 0)
		{
			memcpy(substr3, buf+pmatch[3].rm_so, pmatch[3].rm_eo - pmatch[3].rm_so);
            substr3[pmatch[3].rm_eo - pmatch[3].rm_so] = '\0';
		}
		if(pmatch[4].rm_so >= 0)
		{
			memcpy(substr4, buf+pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so);
            substr4[pmatch[4].rm_eo - pmatch[4].rm_so] = '\0';
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
    char *origin = "sftp://username@localhost:222/tmp/test.txt";
    //char *format = "^1,4,\\([0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\)\\(:[0-9]\\+\\)\\?";
    char *format = "^sftp://\\([^@]\\+\\)@\\([^:/]\\+\\)\\(:[0-9]\\+\\)\\?\\(/.*\\)$";
    char str[4][64] = {{0}};
    char *user = NULL;
    char *host = NULL;
    int port = 22;
    char *path = NULL;

    int rc = match(origin, format, str[0], str[1], str[2], str[3]);
    printf("found %s and %s and %s and %s return %d\n", str[0], str[1], str[2], str[3], rc);
    if (rc == 0)
    {
        user = str[0];
        host = str[1];
        if (str[2][0] == 0)
        {
            port = 22;
        }
        else
        {
            port = atoi(&str[2][1]);
        }
        path = str[3];
    }
    printf("Found user: %s host: %s port: %d path: %s\n", user, host, port, path);
    return 0;
}

