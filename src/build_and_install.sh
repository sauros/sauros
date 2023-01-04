#!/bin/bash

BUILD_FLAG="-DCMAKE_BUILD_TYPE=Release"

function build_and_install() {
   mkdir -p build/${1}
   cd build/${1}
   cmake ${BUILD_FLAG} ../../${1}
   make -j5 
   sudo make install
   cd ../../
}

for i in "$@"
do
case $i in
    -c|--clean)
    rm -rf build
    shift
    ;;
    -d|--debug)
    BUILD_FLAG="-DCMAKE_BUILD_TYPE=Debug"
    shift
    ;;
    -r|--release)
    BUILD_FLAG="-DCMAKE_BUILD_TYPE=Release"
    shift
    ;;
    -p|--profile)
    BUILD_FLAG="-DCMAKE_BUILD_TYPE=Debug -DENABLE_PROFILER=ON"
    shift
    ;;
    -h|--help)
    echo " Options:"
    echo "  -r --release          build release"
    echo "  -d --debug            build debug"
    echo "  -c --clean            clean build directory before compile"
    exit 0
    shift
    ;;
    *)
    # the rest
    ;;
esac
done

build_and_install "libsauros"
build_and_install "app"