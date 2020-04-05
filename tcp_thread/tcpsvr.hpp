#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>



class TcpSvr
{
public:
  TcpSvr()
  {
    _sock = -1;
  }
  ~TcpSvr()
  {
    Close();
  }
  bool CreateSock()
  {
    _sock = socket(AF_INET, SOCK_STREAM, 6);
    if (_sock < 0) {
      perror("create");
      return false;
    }
    return true;
  }

  bool Bind(std::string& ip, uint16_t port)
  {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    ssize_t ret = bind(_sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
      perror("bind");
      return false;
    }
    return true;
  }

  bool Listen(int Backlog)
  {
    ssize_t ret = listen(_sock, Backlog);
    if (ret < 0) {
      perror("listen");
      return false;
    }
    return true;
  }

  bool Accept(TcpSvr* ts, struct sockaddr_in* addr = NULL)
  {
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(struct sockaddr_in);
   int ret = accept(_sock, (struct sockaddr*)&peeraddr, &len);
   if (ret < 0) {
     perror("accept");
     return false;
   }
   ts->_sock = ret;
   if (addr != NULL){
     memcpy(addr, &peeraddr, len);
   }
   return true;
  }

  bool Connect(std::string& ip, uint16_t port)
  {

    struct sockaddr_in destaddr;
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(port);
    destaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    int ret = connect(_sock, (struct sockaddr*)&destaddr, sizeof(destaddr));
    if (ret < 0) {
      perror("connect");
      return 0;
    }
    return true;
  }

  bool Send(std::string& buf)
  {

    int sendSize = send(_sock, buf.c_str(), buf.size(), 0);
    if (sendSize < 0) {
      perror("send");
      return false;
    }
    return true;
  }

  bool Recv(std::string& buffer)
  {
    char buf[1024] = {0};
    //0：阻塞接收
    //MSG_PEEK：探测接收
    int recvSize = recv(_sock, buf, sizeof(buf) - 1, 0);
    if (recvSize < 0) {
      perror("recv");
    }
    else if (recvSize == 0) {
      //如果recvSize等于0，表示对端将连接关闭了
      printf("connect error\n");
      return false;
    }
    buffer.assign(buf, recvSize);
    return true;
  }

  void Close()
  {
    close(_sock);
    _sock = -1;
  }
private:
    int _sock;
};
