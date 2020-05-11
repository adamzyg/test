#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>

int get_lock(const char *path)
{
    int i;
    int rc = -1;
    int lock_fd = -1;
    struct flock lock;

    if (!path)
        return -1;

    lock_fd = open(path, O_WRONLY | O_CREAT, 00644);
    if (lock_fd < 0) {
        fprintf(stderr, "Couldn't open lock file: %s\n", path);
        return -1;
    }

#ifdef FD_CLOEXEC
    fcntl(lock_fd, F_SETFD, fcntl(lock_fd, F_GETFD | FD_CLOEXEC));
#endif
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    for (i = 0; i < 10; i++) {
        rc = fcntl(lock_fd, F_SETLK, &lock);
        if (rc != -1)
            break;

        if (EINTR == errno)
            continue;
        if ((EAGAIN == errno) || (EACCES == errno)) {
            if (i == 0)
                fprintf(stderr, "Warning: try to get lock. Please wait...\n");
            sleep(1);
            continue;
        }
        if (EINVAL == errno)
            fprintf(stderr, "Error: Locking is't supported by OS\n");
        break;
    }

    if (rc == -1) {
        fprintf(stderr, "Error: Couldn't get lock\n");
        close(lock_fd);
        return -1;
    }

    return lock_fd;
}

void release_lock(int lock_fd)
{
    struct flock lock;

    if (lock_fd == -1)
        return;

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    fcntl(lock_fd, F_SETLK, &lock);
    close(lock_fd);
}

int main(void)
{
    unsigned char id = (unsigned char)time(0);
    int i;

    srand(id);
    for (i = 0; i < 20; i++) {
        int fd = get_lock("/tmp/test.lock");
        if (fd < 0) {
            return -1;
        }
        printf("[%x] get lock\n", id);
        sleep(1);
        printf("[%x] release lock\n", id);
        release_lock(fd);
        sleep(rand() & 0x3U);
    }

    return 0;
}
