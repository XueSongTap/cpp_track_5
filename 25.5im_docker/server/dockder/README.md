1. 构建基础镜像

im_base

在0voice_im/server目录运行
cd 0voice_im/server
docker build  -f dockder/im_base/Dockerfile -t im_base:v1.0 .


login_server服务
docker build  -f dockder/login_server/Dockerfile -t login_server:v1.0 .

不用缓存
docker build  -f dockder/login_server/Dockerfile -t login_server:v1.0 --no-cache .


运行：docker run -d   -p 8008:8008 -p 8080:8080 -p 8100:8100 login_server:v1.0
进入容器: docker exec -it  CONTAINER_ID  bash 

2. 其他模块基于基础镜像构建，为了方便debug学习，构建镜像的时候包含了源码文件