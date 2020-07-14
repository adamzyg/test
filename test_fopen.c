#include <unistd.h>
#include <stdio.h>


int main()
{
    FILE *dir = fopen("/sys/class/net/enp2s0", "r");
    if (dir == NULL)
    {
        fprintf(stderr, "interface enp2s0 doesn't exists.\n");
        return -1;
    }
    fclose(dir);
    return 0;
}
