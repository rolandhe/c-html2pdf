//
// Created by xiufeng on 23-6-4.
//

#ifndef HTML2PDF_MY_QUEUE_H
#define HTML2PDF_MY_QUEUE_H


#include "my-lock.h"


typedef struct _safe_queue {
    void ** buf;
    int64_t header;
    int64_t tail;
    int cap;
    my_lock * lock;
} safe_queue;

safe_queue  * init_queue(int cap);
int push(safe_queue *q,void * element);
void * pop(safe_queue* q);
void destroy_queue(safe_queue * q);

#endif //HTML2PDF_MY_QUEUE_H
