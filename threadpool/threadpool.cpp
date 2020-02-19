#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <queue>

typedef void (*Handler_t)(int);

class ThreadTask
{
public:
  ThreadTask()
  {
    _Data = -1;
    _Handler = NULL;
  }
  ThreadTask(int Data, Handler_t Handler)
  {
    _Data = Data;
    _Handler = Handler;
  }

  void run()
  {
    _Handler(_Data);
  }

    ~ThreadTask(){}

private:
  int _Data;
  Handler_t _Handler;


};

#define THREADCOUNT 4
class Threadpool
{
public:
  Threadpool()
    :_Thread_capacity(THREADCOUNT)
    //比较差的一种做法，比较好的方法是线程创建的时候计算
    , _thread_curnum(THREADCOUNT)
  {
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init(&_cond, NULL);
    _is_quit = false;

    //创建线程
    Threadcreate();
  }

private:
  bool Threadcreate()
  {
    int ret = -1;
    pthread_t tid;
    for (int i = 0; i < THREADCOUNT; ++i) {
      ret = pthread_create(&tid, NULL, ThreadStart, NULL);
      if (ret != 0) {
        std::cout << "create thread failed !" << std::endl;
        return false;
      }
    }
    return false;
  }

private:
  //线程池当中初始化的时候，线程的数量；
  size_t _Thread_capacity;
  //当前线程池有多少个数量
  size_t _thread_curnum;
  //线程安全队列
  std::queue<ThreadTask*> _que;
  //互斥
  pthread_mutex_t _mutex;
  //同步机制
  pthread_cond_t _cond;
  //线程退出标志
  bool _is_quit;
};

std::queue<ThreadTask*> que;

int main()
{
  return 0;
}
