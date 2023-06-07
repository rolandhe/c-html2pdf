cmake_minimum_required(VERSION 3.22)
project(html2pdf C)

SET(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_STANDARD 11)

add_executable(html2pdf main.c http-srv.h my-lock.h pdf-thread.h my-lock.c pdf_thread.c my_queue.h my_queue.c convert-pdf.h base64.h base64.c convert_pdf.c thread_pool.h thread_pool.c const_def.h)
target_link_libraries(html2pdf event uuid wkhtmltox)
