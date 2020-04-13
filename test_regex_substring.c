#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

static int endofline(FILE *ifp, int c)
{
    int eol = (c == '\r' || c == '\n');
    if (c == '\r')
    {    
        c = getc(ifp);
        if (c != '\n' && c != EOF) 
            ungetc(c, ifp);
    }    
    return(eol);
}

int get_line(FILE *fp, char *buffer, size_t buflen)
{
    char *end = buffer + buflen - 1; /* Allow space for null terminator */
    char *dst = buffer;
    int c;
    printf("Enter into get_line\n");
    while ((c = getc(fp)) != EOF && !endofline(fp, c) && dst < end) 
    {
        printf("get char %c\n", c);
        if ((*dst++ = c) == '\n')
            break;
    }
    *dst = '\0';
    return((c == EOF && dst == buffer) ? EOF : dst - buffer);
}


int match(char *buf, char *hex_str)
{
    regmatch_t pmatch[2];
    size_t nmatch = 2;
    regex_t reg;
    int rc;
    const char *pattern = ".*TX-C.*\\s\\+\\([0-9]\\+\\)\\s\\+MBytes.*receiver";		// 正则表达式
    //const char *pattern = "\\([0-9]\\+\\)";		// 正则表达式

    if (0 != (rc = regcomp(&reg, pattern, 0)))	//编译正则表达式
    {
        printf("regcomp() failed, returning nonzero (%d)\n", rc);
        return -1;
    }
    int status = regexec(&reg, buf, nmatch, pmatch, 0);
    if(status != 0)
    {
        printf("No Match use pattern %s in buf:%s\n", pattern, buf);
        regfree(&reg);
        return -1;
    }
    else if(pmatch[1].rm_so != -1)
    {
        printf("%d ---- %d\n", pmatch[1].rm_so, pmatch[1].rm_eo);
        memcpy(hex_str, buf+pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
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
    char *str = NULL;
    size_t read_len = 0;
    ssize_t read;
    char hex_str[9] = {0};
    unsigned int hex = 0x0;
    char buf[] = "[  9][TX-C]   0.00-8.42   sec  1000 MBytes   996 Mbits/sec                  receiver";	// 待搜索的字符串
    if (match(buf, hex_str) == 0)
    {
        printf("found string: %s\n", hex_str);
    }
    //FILE *fg = NULL;
    //if ((fg = fopen("/home/zyg/Documents/5G-RU/odu/eeprom.txt", "r")) == NULL)
    //{
    //    printf("Register file doesn't exists.");
    //    return 1;
    //}

    //printf("begin to parse file\n");
    //while( (getline(&str, &read_len, fg)) != -1 )
    //{
    //    printf("getline %s\n", str);
    //    if (match(str, hex_str) != 0)
    //    {
    //        printf("line %s is invalid, can not match the hex regex pattern. Load config to EEPROM failed\n", str);
    //        if(str)
    //            free(str);
    //        fclose(fg);
    //        exit(1);
    //    }
    //    hex = (unsigned int)strtoul(hex_str, NULL, 16);
    //    printf("Get hex %x\n", hex);
	//}
    //if(str)
    //    free(str);
    //fclose(fg);
    
    return 0;
}
