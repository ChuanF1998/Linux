#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

int main()
{
    int fd = open("./tmpfile", O_RDWR | O_CREAT, 0664);
    if (fd < 0) {
        perror("open");
        return 0;
    }
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    FD_SET(fd, &readfds);
    //监控
    int ret = select(fd + 1, &readfds, NULL, NULL, NULL);
    if (ret > 0) {
        if (FD_ISSET(0, &readfds)) {
            char buf[1024] = {0};
            read(0, buf, sizeof(buf) - 1);
            printf("buf:%s\n", buf);
        }
        else if (FD_ISSET(fd, &readfds)) {
            printf("qwew\n");
        }
    }

    FD_CLR(0, &readfds);
    return 0;
}
