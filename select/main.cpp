#define CHECK_RET(p) if (p != true){return 0;}
#include "tcpsvr.hpp"
#include "selectSvr.hpp"


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
    
    //创建select服务
    selectSvr st;
    st.Add(listen_tcpsvr.GetFd());

    while (1) {
        //监控起来
        std::vector<TcpSvr> vec;
        if (!st.SelectWait(&vec, 3000)) {
            continue;
        }
        
        for (int i = 0; i < vec.size(); ++i) {
            if (vec[i].GetFd() == listen_tcpsvr.GetFd()) {
                //有连接
                TcpSvr peerts;
                struct sockaddr_in peeraddr;
                if (!listen_tcpsvr.Accept(peerts, &peeraddr)) {

                    printf("Accept failed !\n");
                    continue;
                }
                printf("Have new connect: ip:%s port:%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
                st.Add(peerts.GetFd());
            }
            else {
                printf("recv data from %d\n", vec[i].GetFd());
                std::string buf;
                if (!vec[i].Recv(buf)) {
                    //接收失败，可能对端已经关闭
                    st.Delete(vec[i].GetFd());
                    vec[i].Close();
                }

                printf("recv data %s from %d\n", buf.c_str(), vec[i].GetFd());
            }
        }

    }
    listen_tcpsvr.Close();
    return 0;
}
