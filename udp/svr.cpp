#include "udpser.hpp"



//ip port
//命令行参数的方式获取
//./svr ip port

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    printf("./svr [ip] [port]\n");
  }
  std::string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  UdpSvr us;
  if (!us.CreateSocket()) {
    return 0;
  }

  if (!us.Bind(ip, port)) {
    return 0;
  }

  std::string buf;
  struct sockaddr_in addr;
  while (1) {
    us.Recv(&addr, buf);
    printf("client say: [%s]\n", buf.c_str());
    printf("server say: ");
    fflush(stdout);

    std::cin >> buf;
    //发送数据给客户端
    us.Send(buf, &addr);
  }
  us.Close();
  return 0;
}
