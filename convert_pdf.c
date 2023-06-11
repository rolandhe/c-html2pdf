//
// Created by xiufeng on 23-6-4.
//
#include "headers/convert-pdf.h"

#include <uuid/uuid.h>
#include <stdio.h>
#include <string.h>
#include "headers/base64.h"
#include "headers/my_queue.h"
#include <event.h>
//for http
#include <evhttp.h>


convert_task * create_task(struct evhttp_request *req){
    convert_task * task = (convert_task*) malloc(sizeof (convert_task));

    struct evkeyvalq* headers = evhttp_request_get_input_headers(req);
    if (headers){
        task->trace_id_ref = evhttp_find_header(headers,"Trace-Id");
    }

    task->req = req;

    task->pdf_len = 0;
    return task;
}
void destroy_task(convert_task * task){
    if (task->pdf_len > 0){
        free(task->pdf_base64);
    }
    free(task);
}

wk_global* init_convert_pdf(){
    wkhtmltopdf_init(0);
    wk_global* g_info = (wk_global*) malloc(sizeof (wk_global));
    g_info->out_setting = wkhtmltopdf_create_global_settings();
    g_info->in_setting = wkhtmltopdf_create_object_settings();
    wkhtmltopdf_set_object_setting(g_info->in_setting, "documentTitle", "documentTitle");
    wkhtmltopdf_set_object_setting(g_info->in_setting, "size.pageSize", "A4");
    wkhtmltopdf_set_object_setting(g_info->in_setting, "orientation", "Portrait");
    wkhtmltopdf_set_object_setting(g_info->in_setting, "margin.top", "1cm");
    wkhtmltopdf_set_object_setting(g_info->in_setting, "margin.bottom", "1cm");
    wkhtmltopdf_set_object_setting(g_info->in_setting, "margin.left", "10mm");
    wkhtmltopdf_set_object_setting(g_info->in_setting, "margin.right", "10mm");
    return g_info;
}

void build_path(char *path,char * ext){
    uuid_t bin_uuid;
    uuid_generate_random(bin_uuid);
    char s_uuid[37];
    uuid_unparse_upper(bin_uuid,s_uuid);
    sprintf(path,"/tmp/%s.%s",s_uuid,ext);
}

void save_html(char * html,int size,char* path){
    build_path(path,"html");
    FILE * out = fopen( path, "w");
    fwrite(html,sizeof(char),size,out);
    fclose(out);
}

void convert_pdf(convert_task * task,wk_global * g_info,safe_queue * dispatch){
    char html_path[512];
    html_path[0] = '\0';

    char *post_data = evbuffer_pullup(task->req->input_buffer,-1);
    save_html(post_data, task->req->body_size,html_path);
    wkhtmltopdf_set_object_setting(g_info->in_setting, "page", html_path);
    wkhtmltopdf_converter * c = wkhtmltopdf_create_converter(g_info->out_setting);
    wkhtmltopdf_add_object(c, g_info->in_setting, NULL);

    /* Perform the actual conversion */
    if (!wkhtmltopdf_convert(c)){
        if(task->trace_id_ref){
            printf("trace_id:%s,message: %s\n",task->trace_id_ref,"Conversion failed!");
        } else{
            printf("message: %s\n","Conversion failed!");
        }
    }

    /* Output possible http error code encountered */
#ifdef _USING_TRACE_MODE_
    if(task->trace_id_ref){
        printf("trace_id:%s,httpErrorCode: %d\n",task->trace_id_ref, wkhtmltopdf_http_error_code(c));
    } else{
        printf("httpErrorCode: %d\n", wkhtmltopdf_http_error_code(c));
    }
#endif

    const unsigned char * pdf_data;
    long pdf_size = wkhtmltopdf_get_output(c,&pdf_data);

    int b64_len = Base64encode_len(pdf_size);
    char * base64_data = malloc(b64_len+1);
    int base64_len =  Base64encode(base64_data,pdf_data,pdf_size);
    task->pdf_base64 = base64_data;
    task->pdf_len = base64_len;

    if(task->trace_id_ref){
        printf("trace_id:%s,post %ld,src %ld pdf data: size %ld, base64 %d\n",task->trace_id_ref, strlen(post_data),task->req->body_size,pdf_size,base64_len);
    } else{
        printf("post %ld,src %ld pdf data: size %ld, base64 %d\n", strlen(post_data),task->req->body_size,pdf_size,base64_len);
    }


    /* Destroy the converter object since we are done with it */
    wkhtmltopdf_destroy_converter(c);

    remove(html_path);
//    remove(pdf_path);
    push(dispatch,task);

}