#!/bin/bash

# create library directory
mkdir ./lib

# build glew
make -C ./glew
mkdir -p ./lib/GLEW/tmp && cp -rf ./GLEW/tmp/* ./lib/GLEW/tmp
# cp -rf ./GLEW/tmp ./lib/GLEW
mkdir -p ./lib/GLEW/lib && cp -rf ./GLEW/lib/* ./lib/GLEW/lib
# cp -rf ./GLEW/lib ./lib/GLEW
mkdir -p ./lib/GLEW/bin && cp -rf ./GLEW/bin/* ./lib/GLEW/bin
# cp -rf ./GLEW/bin ./lib/GLEW

cp -rf ./GLEW/glew.pc ./lib/GLEW/glew.pc

# build된 파일만 쏙! 나머지 자원은 정리.
make clean -C ./GLEW

# build glfw
cmake -S ./GLFW -B ./lib/GLFW/build
make -C ./lib/GLFW/build
