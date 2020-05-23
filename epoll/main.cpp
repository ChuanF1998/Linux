#define CHECK_RET(p) if (p != true){return 0;}
#include "tcpsvr.hpp"
#include "epollSvr.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("[ip] [port]\n");
        return 0;
    }

    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);

    TcpSvr listen_tcpsvr;
    CHECK_RET(listen_tcpsvr.CreateSock());
    CHECK_RET(listen_tcpsvr.Bind(ip, port));
    CHECK_RET(listen_tcpsvr.Listen(5));
    
    EpollSvr es;
    es.Add(listen_tcpsvr.GetFd());

    while (1) {
        std::vector<TcpSvr> output;
        if (!es.EpollWait(&output)) {
            continue;
        }

        for (size_t i = 0; i < output.size(); ++i) {
            //1.有新连接到来
            //  需要调用accept函数
            //  将接收的套接字描述符添加到epoll中
            if(output[i].GetFd() == listen_tcpsvr.GetFd()) {
                TcpSvr newts;
                struct sockaddr_in peeraddr;
                listen_tcpsvr.Accept(newts, &peeraddr);
                printf ("have a new connection %s : %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
                es.Add(newts.GetFd());
            }
            //2.老连接上有数据到来
            else {
                //接收数据
                std::string buf;
                int ret = output[i].Recv(buf);
                if (ret < 0) {
                    printf ("qwer\n");
                    es.DeleteFd(output[i].GetFd());
                    output[i].Close();
                    continue;
                }
                printf ("recv data is %s\n", buf.c_str());
            }
        }
    }


    listen_tcpsvr.Close();
    return 0;
}
