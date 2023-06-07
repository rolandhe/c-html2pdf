//
// Created by xiufeng on 23-6-4.
//

#include "pdf-thread.h"
#include <malloc.h>
#include <pthread.h>
#include "convert-pdf.h"


void convert_pdf_func(pdf_thread_info *info){
    wk_global * g_info = init_convert_pdf();
    signal_wait_one(info->waiter);
    while (1){
        void * elem = pop(info->q);
        convert_task * task = (convert_task*)elem;
        convert_pdf(task,g_info,info->result_dispatch);
    }
}

void start_pdf_thread(pdf_thread_info * info){
    pthread_t thread;
    pthread_create(&thread,NULL,(void*)convert_pdf_func,info);
    pthread_detach(thread);
    signal_wait_one(info->waiter);
}


pdf_thread_info  * init_pdf_thread_info(int cap,safe_queue * dispatch){
    pdf_thread_info *info = (pdf_thread_info*) malloc(sizeof (pdf_thread_info));
    info->q = init_queue(cap);
    info->result_dispatch = dispatch;
    info->waiter = init_wait_one();
    return info;
}
void destroy_pdf_thread_info(pdf_thread_info * info){
    destroy_queue(info->q);
    destroy_wait_one(info->waiter);
    free(info);
}