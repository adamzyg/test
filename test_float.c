#include <stdio.h>
int main()
{
    int count = 150;
    const int cfg_line = 680;
    char * reg_file = "eeprom.txt";
    float progress = 0.0;
    if (count % 50 == 0)
    {
        progress = (float)count / cfg_line * 100;
        printf("Parsing the config file %s %.2f%%\n", reg_file, progress);
    }
}

