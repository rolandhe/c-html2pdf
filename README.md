# c-html2pdf

使用libevent 作为http server封装 wkhtmltox组件，用于转换html成为pdf。
直接post  包含 html 的json 字符串，服务接收到后转换成pdf流，pdf流被转换成base64后封装成json输出。

post json格式：

```
  
  {
       "body":"html"
  }
```

输出json格式：

```
   {
       "success":true,
       "code":200,
       "message":"ok",
       "data":"base64 pdf data"
   }
```

# 运行环境
仅在ubuntu 22.04做过测试。

您可以使用docker运行：
* 打包
* 
```
docker build -t c-html2pdf:1.0  .
```

* 运行

```
docker run -t -p 8087:8080 c-html2pdf:1.0 
```
