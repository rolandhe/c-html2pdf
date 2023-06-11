//
// Created by xiufeng on 23-6-6.
//

#include "headers/thread_pool.h"
#include "headers/convert-pdf.h"
#include <event.h>
#include <unistd.h>
//for http
#include <evhttp.h>
#include "headers/const_def.h"
#include <json-c/json.h>

void logger(const char * trace_id, const char * message){
    if (trace_id){
        printf("trace_id:%s,message:%s\n",trace_id,message);
    } else{
        printf("message:%s\n",message);
    }
}

json_object * task_to_json(convert_task* task){
    struct json_object *jobj;
    jobj = json_object_new_object();
    json_object_object_add(jobj, "success", json_object_new_boolean(task->pdf_len>0));
    if (task->pdf_len == 0){
        json_object_object_add(jobj, "code", json_object_new_int(5500));
        json_object_object_add(jobj, "message", json_object_new_string("convert pdf failed"));
    } else {
        json_object_object_add(jobj, "code", json_object_new_int(200));
        json_object_object_add(jobj, "data", json_object_new_string(task->pdf_base64));
    }
    return jobj;
}

void output_http(safe_queue * q){
    while (1){
        void * elem = pop(q);
        convert_task * task = (convert_task*)elem;
        struct evhttp_request *req = task->req;

        evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
        evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
        evhttp_add_header(req->output_headers, "Connection", "keep-alive");
        pid_t pid = getpid();
        char pid_data[128];
        sprintf(pid_data,"pid:%d",pid);
        evhttp_add_header(req->output_headers, "pid", pid_data);
        //输出的内容
        struct evbuffer *buf;
        buf = evbuffer_new();
        struct json_object *result_json = task_to_json(task);
        const char * json_str = json_object_to_json_string_ext(result_json,JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
#ifdef _USING_TRACE_MODE_
        if (task->pdf_len > 0){
            logger(task->trace_id_ref,task->pdf_base64);
        }
        logger(task->trace_id_ref,json_str);
#endif
        evbuffer_add_printf(buf, "%s",json_str);

        json_object_put(result_json); // Delete the json object

        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        evbuffer_free(buf);
        destroy_task(task);
    }
}



void start_pool(threed_pool * pool){
    for (int i = 0; i < pool->size;i++){
        pthread_t * pt = pool->pooled_threads+i;
        pthread_create(pt,NULL,(void*)output_http,pool->q);
        pthread_detach(*pt);
    }
}

threed_pool * init_pool(int size, int q_cap){
    threed_pool * pool = (threed_pool*) malloc(sizeof(threed_pool));
    pool->pooled_threads = (pthread_t*) malloc(sizeof (pthread_t)*size);
    pool->size = size;
    pool->q = init_queue(q_cap);
    return pool;
}

void destroy_pool(threed_pool * pool){
    for(int i = 0; i <  pool->size;i++){
        pthread_t *pt = pool->pooled_threads + i;
        pthread_cancel(*pt);
    }
    free(pool->pooled_threads);
    destroy_queue(pool->q);
    free(pool);
}