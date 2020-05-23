#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include "tcpsvr.hpp"

class EpollSvr
{

    public:
        EpollSvr()
        {
            _epoll_fd = epoll_create(10);

        }

        bool Add(int fd)
        {
            struct epoll_event ev;
            //关心可读事件
            ev.events = EPOLLIN;
            ev.data.fd = fd;
            int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
            if (ret < 0) {
                perror("epoll_ctl Add failed");
                return false;
            }
            return true;
        }

        bool DeleteFd(int fd)
        {
            int res = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            if (res < 0) {
                perror("epoll_ctl Del failed");
                return false;
            }

            return true;
        }

        bool EpollWait(std::vector<TcpSvr>* output)
        {
            struct epoll_event events[1000];
            int nfds = epoll_wait(_epoll_fd, events, sizeof(events) / sizeof(events[1000]), -1);
            if (nfds < 0) {
                perror("epoll_wait failed");
                return false;
            }

            for (int i = 0; i < nfds; ++i) {
                TcpSvr ts;
                ts.SetFd(events[i].data.fd);
                output->push_back(ts);
            }

            return true;
        }
    private:
        //epoll操作句柄
        int _epoll_fd;
};
