#include <stdio.h>
#include <uuid/uuid.h>

#include "http-srv.h"
#include "pdf-thread.h"


int main() {
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    //默认参数
    char *httpd_option_listen = "0.0.0.0";
    int httpd_option_port = 8080;
    int httpd_option_daemon = 0;
    int httpd_option_timeout = 120; //in seconds




//    //判断是否设置了-d，以daemon运行
//    if (httpd_option_daemon) {
//        pid_t pid;
//        pid = fork();
//        if (pid < 0) {
//            perror("fork failed");
//            exit(EXIT_FAILURE);
//        }
//        if (pid > 0) {
//            //生成子进程成功，退出父进程
//            exit(EXIT_SUCCESS);
//        }
//    }

    /* 使用libevent创建HTTP Server */


    //初始化event API
    event_init();

    //创建一个http server
    struct evhttp *httpd;
    httpd = evhttp_start(httpd_option_listen, httpd_option_port);
    evhttp_set_timeout(httpd, httpd_option_timeout);


    pdf_thread_info * thread_info = init_pdf_thread_info(10);
    start_pdf_thread(thread_info);
    //指定generic callback
    evhttp_set_cb(httpd,"/test", httpd_handler, NULL);
    //也可以为特定的URI指定callback
    evhttp_set_cb(httpd, "/pdf", pdf_handler, thread_info->q);

    //循环处理events
    event_dispatch();

    destroy_pdf_thread_info(thread_info);
    evhttp_free(httpd);

    return 0;
}