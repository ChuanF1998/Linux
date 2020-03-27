#include "udpser.hpp"



int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    printf("./cli [ip] [port]\n");
  }

  //服务端
  std::string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  UdpSvr us;
  if (!us.CreateSocket()) {
    return 0;
  }


  std::string buf;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip.c_str());

  while (1) {
    printf("client say: ");
    std::cin >> buf;
    //发送数据给服务端
    us.Send(buf, &addr);

    fflush(stdout);
    us.Recv(&addr, buf);
    printf("server say: [%s]\n", buf.c_str());
  }

  us.Close();
  return 0;
}
