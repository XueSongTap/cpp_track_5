#!/bin/bash


build() {
    export CPLUS_INCLUDE_PATH=$PWD/slog
    export LD_LIBRARY_PATH=$PWD/slog
    export LIBRARY_PATH=$PWD/slog:$LIBRARY_PATH


    apt-get -y install cmake
    apt-get -y install libuu-dev 
    apt-get -y install libcurl4-openssl-dev
    apt-get -y install libssl-dev
    apt-get -y install libcurl-dev 

    echo "#ifndef __VERSION_H__" > base/version.h
    echo "#define __VERSION_H__" >> base/version.h
    echo "#define VERSION \"$1\"" >> base/version.h
    echo "#endif" >> base/version.h

    if [ ! -d lib ]
    then
        mkdir lib
    fi

    CURPWD=$PWD	

    for i in base slog; do     
        cd $CURPWD/$i
        cmake .
        make
        if [ $? -eq 0 ]; then
            echo "make $i successed";
        else
            echo "make $i failed";
            exit;
        fi
    done

    cd $CURPWD

    cp base/libbase.a lib/

    mkdir base/slog/lib
    cp slog/slog_api.h base/slog/
    cp slog/libslog.so base/slog/lib/
} 

print_help() {
    echo "Usage: "
    echo "  $0 version version_str --- build a version"
}

case $1 in
    version)
        if [ $# != 2 ]; then 
            echo $#
            print_help
            exit
        fi

        echo $2
        echo "build..."
        build $2
        ;;
    *)
    print_help
    ;;
esac



