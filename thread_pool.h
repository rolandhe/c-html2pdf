//
// Created by xiufeng on 23-6-6.
//

#ifndef HTML2PDF_THREAD_POOL_H
#define HTML2PDF_THREAD_POOL_H

#include <pthread.h>
#include <malloc.h>
#include "my_queue.h"
typedef struct _threed_pool{
    pthread_t *pooled_threads;
    int size;
    safe_queue * q;
} threed_pool;

threed_pool * init_pool(int size, int q_cap);

void start_pool(threed_pool * pool);

void destroy_pool(threed_pool * pool);

#endif //HTML2PDF_THREAD_POOL_H
