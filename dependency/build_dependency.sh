#!/bin/bash

# (1) build GLEW      # https://github.com/nigels-com/glew
  cd ./GLEW/build
  cmake ./cmake && make glew_s # build the glew static library
  cd ../../
# (2) build GLFW      # https://github.com/glfw/glfw
  cmake -S ./GLFW -B ./GLFW/build && make -C ./GLFW/build
# (3) build spdlog    # https://github.com/gabime/spdlog
  cmake -S ./spdlog -B ./spdlog/build && make -C ./spdlog/build -j

