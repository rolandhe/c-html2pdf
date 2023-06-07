//
// Created by xiufeng on 23-6-4.
//
#include "convert-pdf.h"

#include <uuid/uuid.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "base64.h"
#include "my_queue.h"
#include <event.h>
//for http
#include <evhttp.h>

convert_task * create_task(struct evhttp_request *req){
    convert_task * task = (convert_task*) malloc(sizeof (convert_task));

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

//char * read_pdf_base64(char * pdf_path,int* base64_len){
//    struct stat file_info;
//    stat(pdf_path,&file_info);
//    char * content = malloc(file_info.st_size);
//    FILE * file = fopen(pdf_path,"r");
//
//    fread(content,sizeof (char),file_info.st_size,file);
//    fclose(file);
//
//    int b64_len = Base64encode_len(file_info.st_size);
//    char * base64_data = malloc(b64_len+1);
//
//    int b_len =  Base64encode(base64_data,content,file_info.st_size);
//    printf("base64 encoded len:%d\n",b_len);
//    free(content);
//    *base64_len = b_len;
//    return base64_data;
//}

void convert_pdf(convert_task * task,wk_global * g_info,safe_queue * dispatch){
    char html_path[512];
//    char pdf_path[512];
    html_path[0] = '\0';
//    pdf_path[0] = '\0';


    char *post_data = evbuffer_pullup(task->req->input_buffer,-1);
    save_html(post_data, task->req->body_size,html_path);
//    build_path(pdf_path,"pdf");

    /* We want the result to be storred in the file called test.pdf */
//    wkhtmltopdf_set_global_setting(g_info->out_setting, "out", pdf_path);

    /* We want to convert to convert the qstring documentation page */
    wkhtmltopdf_set_object_setting(g_info->in_setting, "page", html_path);

    /* Create the actual converter object used to convert the pages */
    wkhtmltopdf_converter * c = wkhtmltopdf_create_converter(g_info->out_setting);

    /*
     * Add the the settings object describing the qstring documentation page
     * to the list of pages to convert. Objects are converted in the order in which
     * they are added
     */
    wkhtmltopdf_add_object(c, g_info->in_setting, NULL);

    /* Perform the actual conversion */
    if (!wkhtmltopdf_convert(c)){
        printf("Conversion failed!");
    }

    /* Output possible http error code encountered */
    printf("httpErrorCode: %d\n", wkhtmltopdf_http_error_code(c));
    char * pdf_data;
    long pdf_size = wkhtmltopdf_get_output(c,&pdf_data);

    int b64_len = Base64encode_len(pdf_size);
    char * base64_data = malloc(b64_len+1);
    int base64_len =  Base64encode(base64_data,pdf_data,pdf_size);
//    char * base64_data = read_pdf_base64(pdf_path,&base64_len);
    task->pdf_base64 = base64_data;
    task->pdf_len = base64_len;

    printf("post %d,src %d pdf data: size %d, base64 %d\n", strlen(post_data),task->req->body_size,pdf_size,base64_len);


    /* Destroy the converter object since we are done with it */
    wkhtmltopdf_destroy_converter(c);

    remove(html_path);
//    remove(pdf_path);
    push(dispatch,task);

}