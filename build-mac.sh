#!/bin/sh
build() {
    local arch=${1}
    local path=${2}
    rm -f ${path}
    echo Building ${path}...
    echo Building Lua...
    gcc ${arch} Lua/*.c -c -O3 -fPIC
    echo Building Apollo...
    g++ ${arch} Apollo/apollo_*.cpp -std=c++11 -c -O3 -fPIC
    local objs=""
    for obj in *.o;
    do
        objs="${objs} ${obj}"
    done
    echo Combining into a dylib...
    g++ ${arch} ${objs} -O3 -fPIC -shared -o ${path}
}
#build -m32 Apollo.gmx/extensions/Apollo/Apollo.dylib
build -m64 Apollo_23/extensions/Apollo/Apollo.dylib
cp Apollo_23/extensions/Apollo/Apollo.dylib Apollo_yy/extensions/Apollo/Apollo.dylib
