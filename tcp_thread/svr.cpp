#include "tcpsvr.hpp"
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

void* ThreadStart(void* arg)
{
  pthread_detach(pthread_self());
  TcpSvr* ts = (TcpSvr*)arg;
  while(1) {
    std::string buf;
    ts->Recv(buf);
    printf("cli say: %s\n", buf.c_str());
    printf("svr say: ");
    fflush(stdout);
    std::cin >> buf;
    ts->Send(buf);
  }
  ts->Close();
  delete ts;
}

void sigcd(int signo)
{
  while (1) {
    (void)signo;
    int ret = waitpid(-1, NULL, WNOHANG);
    if (ret == 0) {
      break;
    }
  }
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    printf("./svr [ip] [port]\n");
    return 0;
  }

  signal(SIGCHLD, sigcd);
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
    TcpSvr* peerts = new TcpSvr();
    struct sockaddr_in peeraddr;
    if (!ts.Accept(peerts, &peeraddr))
    {
      continue;
    }

    pthread_t pid;
    int ret = pthread_create(&pid, NULL, ThreadStart, (void*)peerts);
    if (ret < 0) {
      perror("pthread_create");
      return 0;
    }
  }
  ts.Close();
  return 0;
}
