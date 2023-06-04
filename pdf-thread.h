//
// Created by xiufeng on 23-6-4.
//

#include "my-lock.h"
#include "my_queue.h"

#ifndef HTML2PDF_PDF_THREAD_H
#define HTML2PDF_PDF_THREAD_H

typedef struct _pdf_thread_info {
    wait_one * waiter;
    safe_queue * q;
} pdf_thread_info;

pdf_thread_info  * init_pdf_thread_info(int cap);
void destroy_pdf_thread_info(pdf_thread_info * info);

void start_pdf_thread(pdf_thread_info * info);

#endif //HTML2PDF_PDF_THREAD_H
