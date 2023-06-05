//
// Created by xiufeng on 23-6-4.
//

#ifndef HTML2PDF_CONVERT_PDF_H
#define HTML2PDF_CONVERT_PDF_H

#include "my-lock.h"
#include <malloc.h>
#include <wkhtmltox/pdf.h>

typedef struct _convert_task{
  char * html_body;
  unsigned char * pdf_base64;
  int pdf_len;

  wait_one *waiter;
} convert_task;

typedef struct _wk_global {
    wkhtmltopdf_global_settings * gs;
    wkhtmltopdf_object_settings * os;
}wk_global;

convert_task * create_task(char * html);

void destroy_task(convert_task * task);

wk_global * init_convert_pdf();

void convert_pdf(convert_task * task,wk_global * g_info);


#endif //HTML2PDF_CONVERT_PDF_H
