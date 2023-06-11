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
#include <unistd.h>

#include <time.h>
#include "const_def.h"


long get_nano(){
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME,&spec);
    return spec.tv_sec*1000000000  + spec.tv_nsec;
}

void pdf_handler(struct evhttp_request *req, void *arg) {
    safe_queue * q = (safe_queue*)arg;

//    long start = get_nano();

    convert_task *task = create_task(req);

    int ok = push(q,task);
    if (ok){
        return;
    }
//    char *post_data = evbuffer_pullup(req->input_buffer,-1);

    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    evhttp_send_reply(req, 429, "Too Many Requests", NULL);
    evhttp_request_free(req);
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
    sleep(20);
    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");

    //输出的内容
    struct evbuffer *buf;
    buf = evbuffer_new();
//    evbuffer_add_printf(buf, "It works!\n i receive body %s\n",post_data);
    evbuffer_add_printf(buf, "It works!\n");
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);

}
#endif //HTML2PDF_HTTP_SRV_H
