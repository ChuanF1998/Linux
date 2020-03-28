#include "tcpsvr.hpp"




int main(int argc, char* argv[])
{
  if (argc != 3) {
    printf("./ser [ip] [port]");
    return 0;
  }

  std::string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  TcpSvr ts;
  if (!ts.CreateSock()) {
    return 0;
  }
  if (!ts.Bind(ip, port)) {
    return 0;
  }
  if (!ts.Listen(5)) {
    return 0;
  }

  //while (1) {
    TcpSvr peerts;
    if (!ts.Accept(peerts)) {
      return 0;
    }
  while (1){
    std::string buf;
    peerts.Recv(buf);
    printf("cli say: %s\n", buf.c_str());

    printf("svr say: ");
    fflush(stdout);
    std::cin >> buf;

    peerts.Send(buf);
  }
    peerts.Close();
  return 0;
}
