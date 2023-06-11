#FROM public.ecr.aws/ubuntu/ubuntu:22.04_stable
FROM ubuntu:jammy


ENV PATH=$PATH:/usr/local/go/bin

WORKDIR /target

COPY . .
COPY fonts/ /usr/share/fonts/


RUN apt-get update -y
RUN apt-get install -y wget
#RUN wget https://github.com/wkhtmltopdf/packaging/releases/download/0.12.6.1-2/wkhtmltox_0.12.6.1-2.jammy_amd64.deb


RUN apt-get install -y tar


RUN apt-get install -y xfonts-75dpi
RUN apt-get install -y ca-certificates
RUN apt-get install -y fontconfig
RUN apt-get install -y libjpeg-turbo8
RUN apt-get install -y libx11-6
RUN apt-get install -y libxcb1
RUN apt-get install -y libxext6
RUN apt-get install -y libxrender1
RUN apt-get install -y xfonts-base
RUN apt-get install -y gcc
RUN apt-get install -y libuuid1
RUN apt-get install -y uuid-dev
RUN apt-get install -y libevent-2.1-7
RUN apt-get install -y libevent-dev
RUN apt-get install -y libjson-c-dev
RUN apt-get install -y libjson-c5


RUN apt-get install -y cmake



RUN dpkg -i lib/wkhtmltox_0.12.6.1-2.jammy_amd64.deb

RUN ldconfig


RUN sh build.sh

RUN rm -fr lib
RUN rm -fr fonts

EXPOSE 8080
ENTRYPOINT ["/target/build/html2pdf"]