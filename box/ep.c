#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_EVENTS 10

int main() {
    int epoll_fd, file_fd;
    struct epoll_event ev, events[MAX_EVENTS];

    // 打开文件
    file_fd = open("testfile.txt", O_RDONLY | O_NONBLOCK);
    if (file_fd == -1) {
        perror("open");
        return 1;
    }

    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return 1;
    }

    // 设置 epoll 事件，监控文件可读事件（EPOLLIN）
    ev.events = EPOLLIN;
    ev.data.fd = file_fd;

    // 将文件描述符添加到 epoll 实例中
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, file_fd, &ev) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    printf("Waiting for file to be readable...\n");

    while (1) {
        // 等待文件描述符的事件
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;  // 被中断时重新等待
            }
            perror("epoll_wait");
            return 1;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].events & EPOLLIN) {
                // 如果文件可读，读取文件内容
                char buffer[1024];
                ssize_t bytes_read = read(file_fd, buffer, sizeof(buffer));
                if (bytes_read == -1) {
                    perror("read");
                    return 1;
                } else if (bytes_read == 0) {
                    printf("End of file reached.\n");
                    close(file_fd);
                    return 0;
                }

                // 打印读取的数据
                write(STDOUT_FILENO, buffer, bytes_read);
            }
        }
    }

    // 清理工作
    close(epoll_fd);
    close(file_fd);
    return 0;
}

