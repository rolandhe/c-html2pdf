//
// Created by xiufeng on 23-6-4.
//

#ifndef HTML2PDF_BASE64_H
#define HTML2PDF_BASE64_H

int Base64encode_len(int len);
int Base64encode(char * coded_dst, const char *plain_src,int len_plain_src);

int Base64decode_len(const char * coded_src);
int Base64decode(char * plain_dst, const char *coded_src);

#endif //HTML2PDF_BASE64_H
