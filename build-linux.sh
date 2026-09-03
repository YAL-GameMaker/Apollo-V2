#!/bin/sh
build() {
    local arch=${1}
    local path=${2}
    rm -f ${path}
    echo Building ${path}
    gcc ${arch}  \
        Apollo/apollo_*.cpp \
        Lua/*.c \
        -O3 \
        -fPIC \
        -shared -o ${path}
    # Add -std=c++11 if it barks
}
#build -m32 Apollo.gmx/extensions/Apollo/Apollo.so
build -m64 Apollo_23/extensions/Apollo/Apollo.so
cp Apollo_23/extensions/Apollo/Apollo.so Apollo_yy/extensions/Apollo/Apollo.so
