//
// Created by xiufeng on 23-6-4.
//

#include "headers/my-lock.h"
#include <malloc.h>

my_lock * init_lock(){
    my_lock * lock = (my_lock*)malloc(sizeof(my_lock));
    pthread_mutex_init(&lock->mutex,NULL);
    pthread_cond_init(&lock->cond,NULL);
    return lock;
}
void lock(my_lock *lock){
    pthread_mutex_lock(&lock->mutex);
//    lock->lock_state = LOCKED;
}
void unlock(my_lock *lock){
    pthread_mutex_unlock(&lock->mutex);
}
void cond_signal(my_lock *lock){
    pthread_cond_signal(&lock->cond);
}
void cond_wait(my_lock *lk){
    pthread_cond_wait(&lk->cond,&lk->mutex);
}
void destroy_lock(my_lock *lock){
    pthread_cond_destroy(&lock->cond);
    pthread_mutex_destroy(&lock->mutex);
    free(lock);
}

wait_one * init_wait_one(){
    wait_one * w = (wait_one*) malloc(sizeof (wait_one));
    sem_init(&w->sema,0,0);
    return w;
}
void signal_wait_one(wait_one *wait){
    sem_post(&wait->sema);
}
void await_wait_one(wait_one * wait){
    sem_wait(&wait->sema);
}
void destroy_wait_one(wait_one *wait){
    sem_destroy(&wait->sema);
    free(wait);
}