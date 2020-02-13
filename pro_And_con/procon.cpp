#include <pthread.h>
#include <stdio.h>
#include <queue>
#include <iostream>

#define THREADCOUNT 4

class BlockQueue
{
  public:
    BlockQueue(int capacity)
      : _capacity(capacity)
    {
      pthread_mutex_init(&q_mutex, NULL);
      pthread_cond_init(&_consumecond, NULL);
      pthread_cond_init(&_producecond, NULL);
    }

    bool Pop(int* Data)
    {
      pthread_mutex_lock(&q_mutex);

      *Data = _queue.front();
      while (_queue.empty()) {
        pthread_cond_wait(&_consumecond, &q_mutex);

      }
      _queue.pop();
      pthread_mutex_unlock(&q_mutex);
      pthread_cond_signal(&_producecond);
      return true;
    }


    bool Push(int& Data)
    {
      pthread_mutex_lock(&q_mutex);
      while (isFull()) {
        pthread_cond_wait(&_producecond, &q_mutex);
      }
      _queue.push(Data);
      pthread_mutex_unlock(&q_mutex);
      pthread_cond_signal(&_consumecond);
    }

    bool isFull()
    {
      if (_queue.size() == _capacity) {
        return true;
      }
      return false;
    }


    ~BlockQueue()
    {
      pthread_mutex_destroy(&q_mutex);
      pthread_cond_destroy(&_consumecond);
      pthread_cond_destroy(&_producecond);
    }

  private:
    std::queue<int> _queue;
    //队列容量
    size_t _capacity;
    pthread_mutex_t q_mutex;
    pthread_cond_t _consumecond;
    pthread_cond_t _producecond;
};

void* consumeStart(void* arg)
{
  BlockQueue* bq = (BlockQueue*)arg;
  while(1) {
    int Data;
    bq->Pop(&Data);
    std::cout << "consume " << pthread_self() << " " << Data << std::endl;
  }

  return NULL;
}

void* produceStart(void* arg)
{
  BlockQueue* bq = (BlockQueue*)arg;
  int i = 0;
  while (1) {
    bq->Push(i);
    std::cout << "produce " << pthread_self() << " " << i << std::endl;
    i++;
  }
  return NULL;

}
int main()
{
  pthread_t con_tid[THREADCOUNT], pro_tid[THREADCOUNT];
  BlockQueue* bq = new BlockQueue(5);
  int i = 0;
  int ret = -1;
  for (; i < THREADCOUNT; ++i) {
    ret = pthread_create(&con_tid[i], NULL, consumeStart, (void*)bq);
    if (ret != 0) {
      std::cout << "create failed!" << std::endl;
      return 0;
    }
    ret = pthread_create(&pro_tid[i], NULL, produceStart, (void*)bq);
    if (ret != 0) {
      std::cout << "create failed!" << std::endl;
      return 0;
    }
  }
    for (i = 0; i < THREADCOUNT; ++i) {
      pthread_join(con_tid[i], NULL);
      pthread_join(pro_tid[i], NULL);
    }
    delete bq;
  return 0;
}
