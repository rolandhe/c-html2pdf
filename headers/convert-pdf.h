//
// Created by xiufeng on 23-6-4.
//

#ifndef HTML2PDF_CONVERT_PDF_H
#define HTML2PDF_CONVERT_PDF_H

#include "my-lock.h"
#include "my_queue.h"
#include <malloc.h>
#include <wkhtmltox/pdf.h>

typedef struct _convert_task{
  unsigned char * pdf_base64;
  int pdf_len;
  struct evhttp_request *req;
} convert_task;

typedef struct _wk_global {
    wkhtmltopdf_global_settings * out_setting;
    wkhtmltopdf_object_settings * in_setting;
}wk_global;

convert_task * create_task(struct evhttp_request *req);

void destroy_task(convert_task * task);

wk_global * init_convert_pdf();

void convert_pdf(convert_task * task,wk_global * g_info,safe_queue * dispatch);


#endif //HTML2PDF_CONVERT_PDF_H
