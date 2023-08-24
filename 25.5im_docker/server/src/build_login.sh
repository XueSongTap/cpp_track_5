#!/bin/bash


build() {
    export CPLUS_INCLUDE_PATH=$PWD/slog
    export LD_LIBRARY_PATH=$PWD/slog
    export LIBRARY_PATH=$PWD/slog:$LIBRARY_PATH

    CURPWD=$PWD	
    
    # 编译
    cd $CURPWD/login_server
    rm -rf Makefile CMakeCache.txt CMakeFiles cmake_install.cmake  login_server
    cmake .
    make
    if [ $? -eq 0 ]; then
        echo "make $i successed";
    else
        echo "make $i failed";
        exit;
    fi

    cd ..
    ls -al
    echo $PWD	
    # 创建运行目录
    LOGIN_RUN_DIR=/home/im_run/login_server
    # 拷贝base、log库文件
    mkdir -p $LOGIN_RUN_DIR
    cp slog/log4cxx.properties $LOGIN_RUN_DIR
    cp slog/libslog.so  $LOGIN_RUN_DIR
    cp -a slog/lib/liblog4cxx.so* $LOGIN_RUN_DIR
    # 拷贝login server
    cp login_server/login_server $LOGIN_RUN_DIR
    cp login_server/loginserver.conf $LOGIN_RUN_DIR

    # 删除过程文件
    # cd ..
    # rm -rf /home/im
} 
# 直接执行build就行了
build



