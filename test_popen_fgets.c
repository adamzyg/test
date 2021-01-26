#include<stdio.h>
#include<string.h>


int FpgaCli_Selftest_system_show(char * command)
{
    FILE *fpr = NULL;
	char buff[128] = {0};

    printf("Begin\n");
	memset(buff, 0, 128);
	if((fpr = popen(command, "r")) != NULL)
	{
		while (fgets(buff, 128, fpr) != NULL)
		{
			printf("%s ", buff);
		}
	}
    else
    {
        printf("command %s failed", command);
    }
    if (buff[0] == 0)
    {
        printf("buff[0] == 0\n");
    }
    printf("\n");
    printf("End\n");

	pclose(fpr);
	fpr = NULL;
}

int main()
{
    char *cmd = "./test_printf";
    printf("before\n");
    FpgaCli_Selftest_system_show(cmd);
    printf("after\n");
    return 0;
}

