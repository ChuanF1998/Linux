#include <semaphore.h>
#include <pthread.h>
#include <cstdio>
#include <iostream>
#include <vector>

#define SIZE 4
#define THREADCOUNT 4

class RingQueue
{
public:
  RingQueue()
    :_vec(SIZE)
     , _capacity(SIZE)
     , _pos_write(0)
     , _pos_read(0)
  {
    //初始化同步的信号量
    //生产者生产的时候就看我们有多少空间可以供我们去生产
    sem_init(&_pro_sem, 0, SIZE);
    //消费者初始化的时候是看当前有多少资源可以消费
    sem_init(&_con_sem, 0, 0);
    //初始化互斥
    sem_init(&_lock, 0, 1);
  }

  bool push(int& Data)
  {
    sem_wait(&_pro_sem);
    sem_wait(&_lock);
    _vec[_pos_write] = Data;
    _pos_write =  (_pos_write + 1) % _capacity;
    //资源进行加一操作，唤醒消费者
    sem_post(&_lock);
    sem_post(&_con_sem);
    return true;

  }

  bool pop(int* Data)
  {
    sem_wait(&_con_sem);
    sem_wait(&_lock);
    *Data = _vec[_pos_read];
    _pos_read = (_pos_read + 1) % _capacity;
    sem_post(&_lock);
    sem_post(&_pro_sem);
    return true;
  }

  ~RingQueue()
  {
    sem_destroy(&_pro_sem);
    sem_destroy(&_con_sem);
    sem_destroy(&_lock);
  }
private:
  std::vector<int> _vec;
  size_t _capacity;
  int _pos_write;
  int _pos_read;

  //同步功能的信号量
  //生产者的信号量
  sem_t _pro_sem;
  //消费者信号量
  sem_t _con_sem;
  //锁
  sem_t _lock;

};

void* ProStart(void* arg)
{
  (void)arg;
  RingQueue* tq = (RingQueue*) arg;
  int i = 0;
  while (1) {
    tq->push(i);
    std::cout << "prostart make " << pthread_self() << " " << i << std::endl;
    i++; 
  }
  return NULL;

}

void* ConStart(void* arg)
{
  (void)arg;
  RingQueue* tq = (RingQueue*) arg;
  int Data;
  while (1) {
    tq->pop(&Data);
    std::cout << "constart consume " << pthread_self() << " " << Data << std::endl;
  }
  return NULL;
}

int main()
{
  RingQueue* tq = new RingQueue();
  pthread_t pro_tid[THREADCOUNT], con_tid[THREADCOUNT];
  int i = 0;
  int ret = -1;
  for (i = 0; i < THREADCOUNT; ++i) {
    ret = pthread_create(&pro_tid[i], NULL, ProStart, (void*)tq);
    if (ret != 0) {
      std::cout << "ctreate thread failed !" << std::endl;
      return 0;
    }
  }
  for (i = 0; i < THREADCOUNT; ++i) {
    ret = pthread_create(&con_tid[i], NULL, ConStart, (void*)tq);
    if (ret != 0) {
      std::cout << "ctreate thread failed !" << std::endl;
      return 0;
    }
  }
  for (i = 0; i < THREADCOUNT; ++i) {
    pthread_join(pro_tid[i], NULL);
    pthread_join(con_tid[i], NULL);
  }
  delete tq;
  return 0;
}

