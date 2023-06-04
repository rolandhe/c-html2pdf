//
// Created by xiufeng on 23-6-4.
//

#ifndef HTML2PDF_HTTP_SRV_H
#define HTML2PDF_HTTP_SRV_H
#include <stdlib.h>
#include <event.h>
//for http
#include <evhttp.h>
#include <signal.h>
#include <string.h>
#include "my_queue.h"
#include "convert-pdf.h"

#define MYHTTPD_SIGNATURE   "html2pdf v 0.0.1"

void pdf_handler(struct evhttp_request *req, void *arg) {
    safe_queue * q = (safe_queue*)arg;
    char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
//    printf("%d\n",req->body_size);
//    printf("%d\n",strlen(post_data));
//    printf("%s\n",post_data);

    convert_task *task = create_task(post_data);
//    printf("create task %ld\n",(int64_t)task);
    int ok = push(q,task);
    if (ok){
        await_wait_one(task->waiter);
    }

    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    //输出的内容
    struct evbuffer *buf;
    buf = evbuffer_new();
    if(ok && task->pdf_len > 0){
        evbuffer_add_printf(buf, "%s",task->pdf_base64);
    }else{
        evbuffer_add_printf(buf, "%s\n","failed");
    }
    destroy_task(task);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
}

//当向进程发出SIGTERM/SIGHUP/SIGINT/SIGQUIT的时候，终止event的事件侦听循环
void signal_handler(int sig) {
    switch (sig) {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
            event_loopbreak();  //终止侦听event_dispatch()的事件侦听循环，执行之后的代码
            break;
    }
}

void httpd_handler(struct evhttp_request *req, void *arg) {
    char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
    printf("%ld\n",req->body_size);
    printf("%ld\n",strlen(post_data));

    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    //输出的内容
    struct evbuffer *buf;
    buf = evbuffer_new();
    evbuffer_add_printf(buf, "It works!\n i receive body %s\n",post_data);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);

}
#endif //HTML2PDF_HTTP_SRV_H
