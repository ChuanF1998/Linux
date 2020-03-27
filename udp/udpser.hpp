#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>

class UdpSvr
{
public:
  UdpSvr()
  {
    _socket = -1;
  }
  ~UdpSvr(){}

  //创建套接字
  bool CreateSocket()
  {
    _socket = socket(AF_INET, SOCK_DGRAM, 17);
    if (_socket < 0) {
      perror("socket");
      return false;
    }
    return true;
  }
  
  //绑定地址信息
  bool Bind(std::string& ip, uint16_t port)
  {
    //端口 + ip
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); //两个字节，涉及大小端 
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    int ret = bind(_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
      perror("bind");
      return false;
    }
    return true;
  }

  //发送数据
  bool Send(std::string& buf, sockaddr_in* dest_addr)
  {
    ssize_t sendSize = sendto(_socket, buf.c_str(), buf.size(), 0, (struct sockaddr*)dest_addr, sizeof(struct sockaddr_in));
    if (sendSize < 0) {
      perror("sendto");
      return false;
    }
    return true;
  }

  //接收数据
  bool Recv(struct sockaddr_in* src_addr, std::string& buf)
  {
    char tmp[1024] = {0};
    //socklen_t socklen = sizeof(struct sockaddr_in);
    socklen_t socklen = 0; 
    printf("socklen: [%d]\n", socklen);
    ssize_t ret = recvfrom(_socket, tmp, sizeof(tmp) - 1, 0, (struct sockaddr*)src_addr, &socklen);

    printf("socklen: [%d]\n", socklen);
    if (ret < 0) {
      perror("Recv");
      return false;
    }
    buf.assign(tmp, ret);
    return true;
  }

  //关闭
  void Close()
  {
    close(_socket);
    _socket = -1;
  }

  //socket
  //bind
private:
  int _socket;
};

