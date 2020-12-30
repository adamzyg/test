#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int match(char *buf, char *pattern, char *substr1, char *substr2, char *substr3, char *substr4, char *substr5, char *substr6)
{
    regmatch_t pmatch[7];
    size_t nmatch = 7;
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
		if(pmatch[5].rm_so >= 0)
		{
			memcpy(substr5, buf+pmatch[5].rm_so, pmatch[5].rm_eo - pmatch[5].rm_so);
            substr5[pmatch[5].rm_eo - pmatch[5].rm_so] = '\0';
		}
		if(pmatch[6].rm_so >= 0)
		{
			memcpy(substr6, buf+pmatch[6].rm_so, pmatch[6].rm_eo - pmatch[6].rm_so);
            substr6[pmatch[6].rm_eo - pmatch[6].rm_so] = '\0';
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
    char *origin = "2020-12-28T10:20:30Z";
    //char *format = "^1,4,\\([0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\.[0-9]\\{1\\,3\\}\\)\\(:[0-9]\\+\\)\\?";
    //char *format = "^sftp://\\([^@]\\+\\)@\\([^:/]\\+\\)\\(:[0-9]\\+\\)\\?\\(/.*\\)$";
    char *format = "^\\([0-9]\\{4\\}\\)-\\([0-9]\\{2\\}\\)-\\([0-9]\\{2\\}\\)T\\([0-9]\\{2\\}\\):\\([0-9]\\{2\\}\\):\\([0-9]\\{2\\}\\)Z$";
    char str[6][64] = {{0}};

    int rc = match(origin, format, str[0], str[1], str[2], str[3], str[4], str[5]);
    printf("found %s and %s and %s and %s and %s and %s return %d\n", str[0], str[1], str[2], str[3], str[4], str[5], rc);
    return 0;
}


