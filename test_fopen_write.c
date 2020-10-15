#include <unistd.h>
#include <stdio.h>


int main()
{
    const char *mplane_static = "/tmp/mplane_static";
    FILE *st_file = fopen(mplane_static, "w");
    if (st_file == NULL)
    {
        fprintf(stderr, "open file %s for write failed.\n", mplane_static);
        return -1;
    }
    fprintf(st_file, "1,4,%s:%d\n", "192.168.1.21", 4334);
    fprintf(st_file, "1,4,%s:%d\n", "192.168.2.21", 4334);
    fclose(st_file);
    return 0;
}

