#!/bin/bash

# create library directory
mkdir ./lib

# build glew
make -C ./glew
cp -r -f ./glew/tmp ./lib/glew
cp -r -f ./glew/lib ./lib/glew
cp -r -f ./glew/bin ./lib/glew
cp -r -f ./glew/glew.pc ./lib/glew/glew.pc
# build된 파일만 쏙! 나머지 자원은 정리.
make clean -C ./glew

# build glfw
cmake -S ./glfw -B ./lib/glfw/build
make -C ./lib/glfw/build
