#!/bin/bash

rm ./fdftest
rm ./libfdf.so
rm ./Python/fdf*.so
rm ./fdf.so
rm -rf ./Python/build

rm -rf build
mkdir -p build/Service
mkdir -p build/Photos/Jpeg
mkdir -p build/Photos/All
mkdir -p build/thirdparty/jpeg-data
mkdir -p build/Python/Photos/All
mkdir -p build/Python/Photos/Jpeg
