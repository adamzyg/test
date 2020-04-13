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
    regmatch_t pmatch;
    regex_t reg;
    const char *pattern = "0[xX][0-9a-fA-F]+";		// 正则表达式

    regcomp(&reg, pattern, REG_EXTENDED);	//编译正则表达式
    int status = regexec(&reg, buf, 1, &pmatch, 0);
    if(status == REG_NOMATCH)
    {
        printf("No Match buf:%s\n", buf);
        regfree(&reg);
        return -1;
    }
    else if(pmatch.rm_so != -1)
    {
        memcpy(hex_str, buf+pmatch.rm_so, pmatch.rm_eo - pmatch.rm_so);
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
    char buf[] = "R121  0X0011Fa";	// 待搜索的字符串
    FILE *fg = NULL;
    if ((fg = fopen("/home/zyg/Documents/5G-RU/odu/eeprom.txt", "r")) == NULL)
    {
        printf("Register file doesn't exists.");
        return 1;
    }

    printf("begin to parse file\n");
    while( (getline(&str, &read_len, fg)) != -1 )
    {
        printf("getline %s\n", str);
        if (match(str, hex_str) != 0)
        {
            printf("line %s is invalid, can not match the hex regex pattern. Load config to EEPROM failed\n", str);
            if(str)
                free(str);
            fclose(fg);
            exit(1);
        }
        hex = (unsigned int)strtoul(hex_str, NULL, 16);
        printf("Get hex %x\n", hex);
	}
    if(str)
        free(str);
    fclose(fg);
    
    return 0;
}
