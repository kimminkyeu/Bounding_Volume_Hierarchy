#!/bin/bash

# create library directory
mkdir ./lib

# build glew
make -C ./glew
cp -r -f ./GLEW/tmp ./lib/GLEW
cp -r -f ./GLEW/lib ./lib/GLEW
cp -r -f ./GLEW/bin ./lib/GLEW
cp -r -f ./GLEW/glew.pc ./lib/GLEW/glew.pc
# build된 파일만 쏙! 나머지 자원은 정리.
make clean -C ./GLEW

# build glfw
cmake -S ./GLFW -B ./lib/GLFW/build
make -C ./lib/GLFW/build
