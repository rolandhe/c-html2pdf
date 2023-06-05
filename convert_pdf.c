//
// Created by xiufeng on 23-6-4.
//
#include "convert-pdf.h"

#include <uuid/uuid.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "base64.h"


convert_task * create_task(char * html){
    convert_task * task = (convert_task*) malloc(sizeof (convert_task));
    int l = strlen(html);
    task->html_body = (char*) malloc(l + 1);
    strcpy(task->html_body,html);
    task->pdf_len = 0;
    task->waiter = init_wait_one();
    return task;
}
void destroy_task(convert_task * task){
    if (task->pdf_len > 0){
        free(task->pdf_base64);
    }
    free(task->html_body);
    destroy_wait_one(task->waiter);
    free(task);
}

wk_global* init_convert_pdf(){
    wkhtmltopdf_init(0);
    wk_global* g_info = (wk_global*) malloc(sizeof (wk_global));
    g_info->gs = wkhtmltopdf_create_global_settings();
    g_info->os = wkhtmltopdf_create_object_settings();
    wkhtmltopdf_set_object_setting(g_info->os,"documentTitle","documentTitle");
    wkhtmltopdf_set_object_setting(g_info->os,"size.pageSize","A4");
    wkhtmltopdf_set_object_setting(g_info->os,"orientation","Portrait");
    wkhtmltopdf_set_object_setting(g_info->os,"margin.top","1cm");
    wkhtmltopdf_set_object_setting(g_info->os,"margin.bottom","1cm");
    wkhtmltopdf_set_object_setting(g_info->os,"margin.left","10mm");
    wkhtmltopdf_set_object_setting(g_info->os,"margin.right","10mm");
    return g_info;
}

void build_path(char *path,char * ext){
    uuid_t bin_uuid;
    uuid_generate_random(bin_uuid);
    char s_uuid[37];
    uuid_unparse_upper(bin_uuid,s_uuid);
    sprintf(path,"/tmp/%s.%s",s_uuid,ext);
}

void save_html(char * html,char* path){
    build_path(path,"html");
    FILE * out = fopen( path, "w");
    fwrite(html,sizeof(char),strlen(html),out);
    fclose(out);
}

char * read_pdf_base64(char * pdf_path,int* base64_len){
    struct stat file_info;
    stat(pdf_path,&file_info);
    char * content = malloc(file_info.st_size);
    FILE * file = fopen(pdf_path,"r");

    fread(content,sizeof (char),file_info.st_size,file);
    fclose(file);

    int b64_len = Base64encode_len(file_info.st_size);
    char * base64_data = malloc(b64_len+1);

    int b_len =  Base64encode(base64_data,content,file_info.st_size);
    printf("base64 encoded len:%d\n",b_len);
    free(content);
    *base64_len = b_len;
    return base64_data;
}

void convert_pdf(convert_task * task,wk_global * g_info){
    char html_path[512];
    char pdf_path[512];
    html_path[0] = '\0';
    pdf_path[0] = '\0';

    save_html(task->html_body, html_path);
    build_path(pdf_path,"pdf");


    /* We want the result to be storred in the file called test.pdf */
    wkhtmltopdf_set_global_setting(g_info->gs, "out", pdf_path);



    /* We want to convert to convert the qstring documentation page */
    wkhtmltopdf_set_object_setting(g_info->os, "page", html_path);



    /* Create the actual converter object used to convert the pages */
    wkhtmltopdf_converter * c = wkhtmltopdf_create_converter(g_info->gs);

    /*
     * Add the the settings object describing the qstring documentation page
     * to the list of pages to convert. Objects are converted in the order in which
     * they are added
     */
    wkhtmltopdf_add_object(c, g_info->os, NULL);

    /* Perform the actual conversion */
    if (!wkhtmltopdf_convert(c)){
        printf("Conversion failed!");
    }

    /* Output possible http error code encountered */
    printf("httpErrorCode: %d\n", wkhtmltopdf_http_error_code(c));

    /* Destroy the converter object since we are done with it */
    wkhtmltopdf_destroy_converter(c);

    int base64_len;
    char * base64_data = read_pdf_base64(pdf_path,&base64_len);
    task->pdf_base64 = base64_data;
    task->pdf_len = base64_len;

    remove(html_path);
    remove(pdf_path);

    signal_wait_one(task->waiter);
}