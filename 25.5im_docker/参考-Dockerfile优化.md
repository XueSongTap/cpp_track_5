# dockerfile优化

原文链接：[(138条消息) dockerfile优化_殇275的博客-CSDN博客_dockerfile优化](https://blog.csdn.net/weixin_63634809/article/details/124721535)



文章目录
优化前
优化方法1：不需要输出的指令丢入/dev/null （需要确定命令执行的是正确的）
优化方法2：减少RUN构建
优化方法3：多阶段构建（使用FROM命令生成多个镜像，将指定的镜像做为其他镜像的基础镜像环境来构建）
优化方法4: 使用更为轻量级的linux 发行版本

 

# 优化前

```dockerfile
mkdir /nginx

FROM centos:7
RUN yum install -y gcc pcre pcre-devel devel zlib-devel make
ADD nginx-1.15.9.tar.gz /mnt 
WORKDIR /mnt/nginx-1.15.9
#关闭debug日志
RUN sed -i 's/CFLAGS="$CFLAGS -g"/#CFLAGS="$CFLAGS -g"/g' auto/cc/gcc 
RUN ./configure --prefix=/usr/local/nginx 
RUN make
RUN make install
EXPOSE 80
VOLUME ["/usr/local/nginx/html"]
CMD ["/usr/local/nginx/sbin/nginx","-g","daemon off;"]

docker build -t nginx:v1 .
```



# 优化方法1：不需要输出的指令丢入/dev/null （需要确定命令执行的是正确的）

**减少不必要的缓存（丢到黑洞）删除不必要的缓存文件/磁盘文件**

```dockerfile
FROM centos:7
RUN yum install -y gcc pcre pcre-devel devel zlib-devel make &> /dev/null && yum clean all
ADD nginx-1.15.9.tar.gz /mnt
WORKDIR /mnt/nginx-1.15.9
#关闭debug日志
RUN sed -i 's/CFLAGS="$CFLAGS -g"/#CFLAGS="$CFLAGS -g"/g' auto/cc/gcc
RUN ./configure --prefix=/usr/local/nginx &> /dev/null
RUN make &> /dev/null
RUN make install &> /dev/null
RUN rm -rf /mnt/nginx-1.15.9
EXPOSE 80
VOLUME ["/usr/local/nginx/html"]
CMD ["/usr/local/nginx/sbin/nginx""-g","daemon off;"]

docker build -t nginx:v2 .
```

# 优化方法2：减少RUN构建

**可以使用&&连接**

```dockerfile
FROM centos:7
ADD nginx-1.15.9.tar.gz /mnt 
WORKDIR /mnt/nginx-1.15.9
RUN yum install -y gcc pcre pcre-devel devel zlib-devel make &> /dev/null && \
 yum clean all && \
 sed -i 's/CFLAGS="$CFLAGS -g"/#CFLAGS="$CFLAGS -g"/g' auto/cc/gcc && \
 ./configure --prefix=/usr/local/nginx &> /dev/null && \
 make &> /dev/null && make install &> /dev/null &&\
 rm -rf /mnt/nginx-1.15.9 
EXPOSE 80
VOLUME ["/usr/local/nginx/html"]
CMD ["/usr/local/nginx/sbin/nginx","-g","daemon off;"]

docker build -f Dockerfile -t nginx:v3 .
```



# 优化方法3：多阶段构建（使用FROM命令生成多个镜像，将指定的镜像做为其他镜像的基础镜像环境来构建）



```dockerfile
FROM centos:7 as build 
ADD nginx-1.15.9.tar.gz /mnt 
WORKDIR /mnt/nginx-1.15.9
RUN yum install -y gcc pcre pcre-devel devel zlib-devel make &> /dev/null && \
 yum clean all &&\
 sed -i 's/CFLAGS="$CFLAGS -g"/#CFLAGS="$CFLAGS -g"/g' auto/cc/gcc && \
 ./configure --prefix=/usr/local/nginx &> /dev/null && \
 make &>/dev/null && \
 make install &>/dev/null && \
 rm -rf /mnt/nginx-1.15.9 

FROM centos:7 
EXPOSE 80
VOLUME ["/usr/local/nginx/html"]
COPY --from=build /usr/local/nginx /usr/local/nginx
CMD ["/usr/local/nginx/sbin/nginx","-g","daemon off;"]

docker build -f Dockerfile -t nginx:v4 .
```



# 优化方法4: 使用更为轻量级的linux 发行版本

debian
alpine
apt add

# 优化三次的结果

![在这里插入图片描述](https://img-blog.csdnimg.cn/c961aab0f9fa44b2b2704a1b44adb6d8.png)