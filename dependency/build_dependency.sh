#!/bin/bash

# (0) get ImGui       # https://github.com/ocornut/imgui/tree/docking
  git clone -b docking https://github.com/ocornut/imgui.git

# (1) build GLEW      # https://github.com/nigels-com/glew
# TODO: change to Curl or Wget or Forked Repo!
  cd ./GLEW/build
  cmake ./cmake && make glew_s # build the glew static library
  cd ../../

# (2) build GLFW      # https://github.com/glfw/glfw
# TODO: change to Curl or Wget or Forked Repo!
  cmake -S ./GLFW -B ./GLFW/build && make -C ./GLFW/build

# (3) build spdlog    # https://github.com/gabime/spdlog
# TODO: change to Curl or Wget or Forked Repo!
  cmake -S ./spdlog -B ./spdlog/build && make -C ./spdlog/build -j

# (4) build assimp    # https://github.com/assimp/assimp
# TODO: change to Curl or Wget or Forked Repo!
  cmake -S ./assimp -B ./assimp/build && make -C ./assimp/build -j

