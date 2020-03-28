#include "tcpsvr.hpp"
#include <sys/wait.h>


int main(int argc, char* argv[])
{
  if (argc != 3) {
    printf("./svr [ip] [port]\n");
    return 0;
  }
  
  std::string ip = argv[1];
  uint16_t port = atoi(argv[2]);
  
  TcpSvr ts;
  if (!ts.CreateSock()) {
    return 0;
  }
  if(!ts.Bind(ip, port)) {
    return 0;
  }
  if (!ts.Listen(5)) {
    return 0;
  }

  while (1) {
    //接收连接
    TcpSvr peerts;
    struct sockaddr_in peeraddr;
    if (!ts.Accept(peerts, &peeraddr))
    {
      continue;
    }
    printf("new connect: %s:%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
    //创建子进程
    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
      exit(1);
    }
    else if (pid == 0) {
    //子进程服务
      std::string buf;
      while (1) {

        peerts.Recv(buf);
        printf("cli say: %s\n", buf.c_str());

        printf("svr say:");
        fflush(stdout);
        std::cin >> buf;
        peerts.Send(buf);
      }
      peerts.Close();
      exit(1);
    }
    peerts.Close();
  }
    waitpid(-1, NULL, WUNTRACED);
  return 0;
}
