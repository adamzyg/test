#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include <errno.h>

ssize_t read_line(int fd, void *buffer, size_t n)
{
    ssize_t numRead;    /* # of bytes fetched by last read()*/
    size_t     totRead;    /* Total bytes read so far*/
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL){
        errno = EINVAL;
        return -1;
    }

    buf = buffer;
    totRead = 0;
    for(;;){
        numRead = read(fd, &ch, 1);
        if (numRead == -1){
            if (errno == EINTR)     /*Interrupted ----> restart read()*/
                continue;
            else
                return -1;            /* some other error*/
        }else if (numRead == 0){    /* EOF*/
            if (totRead == 0)        /* No bytes read so far, return 0*/
                return 0;
            else                    /* Some bytes read so far, add '\0'*/
                break;
        }else{
            if (totRead < n-1){        /* Discard > (n-1) bytes */
                totRead++;
                if (totRead == n-2)   /* 让readLine最多能读取n-1个字符，不包括null字符，并且不丢弃字节*/
                    break;
                *buf++=ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}
int main()
{
    char buf[128] = {0};
    char *filepath = "./test_open_read.c";
    int fd = open(filepath, O_RDONLY);
    while((read_line(fd, buf, 128)) != 0)
    {
        printf("%s", buf);
    }
    close(fd);
    return 0;
}
