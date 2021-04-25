#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int i;

    if (argc == 1) {
        fprintf(stderr, "Usage:\n\t%s (file1) (file2) .....\n", argv[0]);
        return -1;
    }

    for (i = 1; i < argc; i++) {
        struct stat stat_record;
        printf("%s - ", argv[i]);
        if (stat(argv[i], &stat_record))
            printf("Can't stat (does file exist?)\n");
        else if (!stat_record.st_size) printf("Empty file\n");
        else {
            /* Now you can open up the file an do what ever it is your going
               to do - since it is non-empty
               (or more technically correct probably still non-empty) */
            printf(" Non-empty file (%d bytes)\n", stat_record.st_size);
        }
    }
    return 0;
}
