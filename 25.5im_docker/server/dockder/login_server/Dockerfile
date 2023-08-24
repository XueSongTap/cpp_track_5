# This is dockerfile for login_server 

# base image 
FROM im_base:v1.0 


COPY ./src/login_server    /home/im/login_server 
COPY ./src/build_login.sh   /home/im/ 
COPY ./dockder/login_server/loginserver.conf /home/im/login_server
# 需要根据配置文件填写
EXPOSE 8008 8080 8100   
WORKDIR "/home/im"
RUN ./build_login.sh


WORKDIR "/home/im_run/login_server"
CMD ["./login_server"]