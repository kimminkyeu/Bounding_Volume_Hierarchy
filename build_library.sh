#!/bin/bash


# (1) on Ubuntu, install build tools first.
# ------------------------------------------------
# sudo apt-get install build-essential libxmu-dev libxi-dev libgl-dev libglu-dev


# (2) on MacOS, ??
# ------------------------------------------------
# 이건 클러스터에서 테스트 진행할 것.


# build GLEW
# https://github.com/nigels-com/glew
# ------------------------------------------------
# (1) build with make
	# make -C ./GLEW
	# sudo make install -C ./GLEW
	# make clean -C ./GLEW
	
# (2) build with cmake
cd ./GLEW/build
cmake ./cmake
make glew_s # build the glew static library
cd ../../


# build GLFW
# ------------------------------------------------
cmake -S ./GLFW -B ./GLFW/build
make -C ./GLFW/build
