//
// Created by xiufeng on 23-6-4.
//

#include "headers/my_queue.h"
#include <malloc.h>

safe_queue  * init_queue(int cap){
    safe_queue * q = (safe_queue*) malloc(sizeof (void*) * cap);
    q->cap = cap;
    q->buf = (void **) malloc(sizeof (void *) * cap);
    q->header = q->tail = 0;
    q->lock = init_lock();
    return q;
}
int push(safe_queue * q,void * element){
    lock(q->lock);
    if(q->tail-q->header >= q->cap){
        unlock(q->lock);
        return 0;
    }
    int index = q->tail % q->cap;
    q->tail++;
    q->buf[index] = element;
    cond_signal(q->lock);
    unlock(q->lock);
    return 1;
}
void * pop(safe_queue * q){
    lock(q->lock);
    while (1){
        if (q->tail - q->header == 0){
            cond_wait(q->lock);
        } else{
            break;
        }
    }
    int index = q->header % q->cap;
    q->header++;
    void * elem = q->buf[index];
    unlock(q->lock);
//    printf("get task %ld\n",(int64_t)elem);
    return elem;
}
void destroy_queue(safe_queue * q){
    destroy_lock(q->lock);
    free(q->buf);
    free(q);
}