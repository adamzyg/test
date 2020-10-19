#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

int main()
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("/tmp/zyg")) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            {
                printf ("%s\n", ent->d_name);
            }
        }
        closedir (dir);
    } else
    {
        /* could not open directory */
        perror ("error");
        return -1;
    }
}
