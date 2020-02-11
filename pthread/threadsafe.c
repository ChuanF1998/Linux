#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define threadcount 4

//定义互斥锁变量
pthread_mutex_t g_mutex;
int g_tickets = 100;
typedef struct ThreadPare
{
  int ThreadId_;
}Tp;

void* ThreadStart(void* arg)
{
  Tp * tp = (Tp*)arg;
  while (1) {
    pthread_mutex_lock(&g_mutex);
    if (g_tickets > 0) {
      printf("thread[%d], tickets is [%d]\n", tp->ThreadId_, g_tickets);
      g_tickets--;
    }
    else {
      pthread_mutex_unlock(&g_mutex);
      delete tp;
      break;
    }
    pthread_mutex_unlock(&g_mutex);
  }
  return NULL;
}




int main()
{
  //线程创建之前初始化互斥锁变量
  pthread_mutex_init(&g_mutex, NULL);
  pthread_t tid[threadcount];
  int i = 0;
  for (; i < threadcount; ++i) {
    Tp* tp = new Tp;
    tp->ThreadId_ = i;
    pthread_create(&tid[i], NULL, ThreadStart, (void*)tp);
  }
  for (i = 0; i < threadcount; ++i) {
    pthread_join(tid[i], NULL);
  }
  pthread_mutex_destroy(&g_mutex);
  return 0;
}
