#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define THREADCOUNT 4 
//定义互斥锁资源
pthread_mutex_t g_mutex;
//定义条件变量
pthread_cond_t g_cond;
pthread_cond_t g_makecond;
//面的数量
int g_noodle = 0;

void* EatStart(void* arg)
{
  (void)arg;
  while (1) {
    pthread_mutex_lock(&g_mutex);
    while (g_noodle == 0) {
      pthread_cond_wait(&g_cond, &g_mutex);
    }
    g_noodle--;
    printf("[%p], eat noodle:[%d]\n", pthread_self(), g_noodle);
    //pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);
    pthread_cond_signal(&g_makecond);
  }
  return NULL;
}

void* MakeStart(void* arg)
{
  (void)arg;
  while (1) {
    pthread_mutex_lock(&g_mutex);
    while (g_noodle == 1) {
      pthread_cond_wait(&g_makecond, &g_mutex);
    }
    g_noodle++;
    printf("[%p], make noodle:[%d]\n", pthread_self(), g_noodle);
    //pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);
    pthread_cond_signal(&g_cond);
  }
  return NULL;
}


int main()
{
  pthread_mutex_init(&g_mutex, NULL);
  //初始化
  pthread_cond_init(&g_cond, NULL);
  pthread_cond_init(&g_makecond, NULL);
  pthread_t eat_tid[THREADCOUNT],make_tid[THREADCOUNT];
  int i = 0;
  int ret = -1;
  for(; i < THREADCOUNT; ++i) {
    ret = pthread_create(&eat_tid[i], NULL, EatStart, NULL);
    if (ret != 0) {
      printf ("create thread failed\n");
      return 0;
    }
  }
  i = 0;
  for(; i < THREADCOUNT; ++i) {
    ret = pthread_create(&make_tid[i], NULL,MakeStart, NULL);
    if (ret != 0) {
      printf ("create thread failed\n");
      return 0;
    }
  }
  for (i = 0; i < THREADCOUNT; ++i) {
    pthread_join(eat_tid[i], NULL);
    pthread_join(make_tid[i], NULL);
  }
  pthread_cond_destroy(&g_cond);
  pthread_cond_destroy(&g_cond);
  pthread_mutex_destroy(&g_mutex);
  return 0;
}
