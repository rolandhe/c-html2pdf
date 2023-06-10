//
// Created by xiufeng on 23-6-4.
//

#ifndef HTML2PDF_MY_LOCK_H
#define HTML2PDF_MY_LOCK_H
#include <pthread.h>
#include <semaphore.h>

//#define LOCKED  1
//#define UN_LOCKED  0

typedef  struct _my_lock{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
//    int lock_state;
} my_lock;

my_lock * init_lock();
void lock(my_lock *lock);
void unlock(my_lock *lock);
void cond_signal(my_lock *lock);
void cond_wait(my_lock *lock);
void destroy_lock(my_lock *lock);

typedef struct _wait_one  {
    sem_t sema;
} wait_one;

wait_one * init_wait_one();
void signal_wait_one(wait_one *wait);
void await_wait_one(wait_one * wait);
void destroy_wait_one(wait_one *wait);

#endif //HTML2PDF_MY_LOCK_H
