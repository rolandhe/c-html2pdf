cmake_minimum_required(VERSION 3.22)
project(html2pdf C)

SET(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_STANDARD 11)
aux_source_directory(. DIR_SRCS)

#add_executable(html2pdf main.c headers/http-srv.h headers/my-lock.h headers/pdf-thread.h my-lock.c pdf_thread.c headers/my_queue.h my_queue.c headers/convert-pdf.h headers/base64.h base64.c convert_pdf.c headers/thread_pool.h thread_pool.c headers/const_def.h)
add_executable(html2pdf ${DIR_SRCS})
target_link_libraries(html2pdf event uuid wkhtmltox)
