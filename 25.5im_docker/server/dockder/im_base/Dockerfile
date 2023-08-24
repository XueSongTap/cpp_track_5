# 构建im_base镜像
# base image  im_base:v1.0
FROM ubuntu:18.04 

LABEL maintainer="Darren 326873713" \
      version="1.0" \
      description="This is im base."

# 安装必要的工具
# 不需要输出的指令丢入/dev/null
RUN apt-get update && apt-get install -y gcc make cmake g++ && \
    mkdir /home/im 
COPY ./src/base  /home/im/base     
COPY ./src/make_log4cxx.sh  /home/im/
COPY ./src/make_protobuf.sh  /home/im/
COPY ./src/build_base.sh  /home/im/
COPY ./src/libsecurity  /home/im/libsecurity
COPY ./src/protobuf  /home/im/protobuf
COPY ./src/log4cxx  /home/im/log4cxx 
COPY ./src/slog  /home/im/slog
COPY ./src/tools  /home/im/tools    
WORKDIR "/home/im/"
RUN  ./make_protobuf.sh 
RUN  ./make_log4cxx.sh  
RUN  ./build_base.sh version 1.0
RUN  apt-get clean

# RUN  ./make_protobuf.sh && \
#     ./make_log4cxx.sh  && \
#     ./build_base.sh version 1.0 &&\
#     RUN  apt-get clean