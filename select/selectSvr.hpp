#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include "tcpsvr.hpp"

class selectSvr
{

    public:
        selectSvr()
        {
            FD_ZERO(&_readfds);
            _max_readfd = -1;
        }

        //添加
        void Add(int fd)
        {
            //添加到集合当中
            FD_SET(fd, &_readfds);
            //更新一下最大的文件描述符
            if (fd > _max_readfd) {
                _max_readfd = fd;
            }
        }

        //删除
        void Delete(int fd)
        {
            //删除
            FD_CLR(fd, &_readfds);
            //更新
            for (int i = _max_readfd; i >= 0; --i) {
                if (!FD_ISSET(i, &_readfds)) {
                    continue;
                }
                _max_readfd = i;
                break;
            }
        }

        //监控
        bool SelectWait(std::vector<TcpSvr>* vec, int usec = 100)
        {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = usec;

            //监控
            fd_set tmp = _readfds;
            int ret = select(_max_readfd + 1, &tmp, NULL, NULL, &tv);
            if(ret < 0) {
                perror("select");
                return false;
            }
            else if (ret == 0) {
                //printf("select timeout\n");
                return false;
            }

            //select返回ret大于0
            for (int i = 0; i <= _max_readfd; ++i) {
                if (FD_ISSET(i, &tmp)) {
                    TcpSvr ts;
                    ts.SetFd(i);
                    vec->push_back(ts);
                }
            }
        
            return true;
        }
    private:
        fd_set _readfds;
        //当前想让select监听的最大文件描述符
        int _max_readfd;
};
