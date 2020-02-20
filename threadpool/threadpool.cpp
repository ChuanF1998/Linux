#include <stdlib.h>
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
    bool is_create = Threadcreate();
    if (!is_create) {
      std::cout << "Thread creare failed !" << std::endl;
      exit(1);
    }
  }

  //放数据
  bool push(ThreadTask* t)
  {
    pthread_mutex_lock(&_mutex);
    if (_is_quit) {
      pthread_mutex_unlock(&_mutex);
      return false;
    }
    _que.push(t);
    pthread_mutex_unlock(&_mutex);
    //放完数据的时候需要通知线程池当中的线程
    pthread_cond_signal(&_cond);
    return true;
  }

  //取数据
  bool pop(ThreadTask** t)
  {
    *t = _que.front();
    _que.pop();
    return true;
  }

  bool PoolClrear()
  {
    pthread_mutex_lock(&_mutex);
    _is_quit = true;
    pthread_mutex_unlock(&_mutex);
    //当线程当中的队列没有数据时，线程都是阻塞在锁中
    if (_thread_curnum > 0) {
      pthread_cond_broadcast(&_cond);
    }
    return true;
  }

  //线程入口函数
private:
  static void* ThreadStart(void* arg)
  {
    Threadpool* t = (Threadpool*)arg;
    while (1) {
      pthread_mutex_lock(&t->_mutex);
      while (t->_que.empty()) {
        //如果不在此位置，则会一直阻塞
        if (t->_is_quit) {
          t->ThreadQuit();
        }
        pthread_cond_wait(&t->_cond, &t->_mutex);
      }
      ThreadTask* v;
      t->pop(&v);
      pthread_mutex_unlock(&t->_mutex);
      //由于已经拿到数据了，所以可以先行把锁释放
      v->run();
    }
    return NULL;
  }

  void ThreadQuit()
  {
    //临界
    _thread_curnum--;
    pthread_mutex_unlock(&_mutex);
    pthread_exit(NULL);
  }
private:
  bool Threadcreate()
  {
    int ret = -1;
    pthread_t tid;
    for (int i = 0; i < THREADCOUNT; ++i) {
      ret = pthread_create(&tid, NULL, ThreadStart, (void*)this);
      if (ret != 0) {
        std::cout << "create thread failed !" << std::endl;
        return false;
      }
    }
    return true;
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

void print(int Data)
{
  std::cout << "I am a Handler, ThreadTask " << Data << std::endl;
}

int main()
{
  Threadpool* t = new Threadpool();
  for (int i = 0; i < 10; ++i) {
    ThreadTask* v = new ThreadTask(i, print);
    t->push(v);
  }
  sleep(5);
  t->PoolClrear();
  delete t;
  return 0;
}
