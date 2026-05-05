#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

void _exit(int status) {
    while (1) {}
}

int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

int _getpid(void) {
    return 1;
}
