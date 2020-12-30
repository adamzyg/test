#include <stdio.h>
#include <unistd.h>

int main()
{
    if (!access("/tmp/zyg/test/ietf-interfaces@2018-02-20.yang", R_OK))
    {
        printf("/tmp/zyg/test/ietf-interfaces@2018-02-20.yang exists\n");
    }
    else
    {
        printf("/tmp/zyg/test/ietf-interfaces@2018-02-20.yang does not exists\n");
    }

    if (!access("/tmp/zyg/test/ietf-interfaces@2019-02-20.yang", R_OK))
    {
        printf("/tmp/zyg/test/ietf-interfaces@2019-02-20.yang exists\n");
    }
    else
    {
        printf("/tmp/zyg/test/ietf-interfaces@2019-02-20.yang does not exists\n");
    }
    return 0;
}
