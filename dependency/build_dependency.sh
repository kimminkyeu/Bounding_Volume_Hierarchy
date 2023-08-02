#!/bin/bash

# (0) get ImGui       # https://github.com/ocornut/imgui/tree/docking
  git clone -b docking https://github.com/ocornut/imgui.git
  rm -rf imgui/.git

# (1) build GLEW      # https://github.com/nigels-com/glew
# TODO: change to Curl or Wget or Forked Repo!
  cd ./GLEW/build
  cmake ./cmake && make glew_s # build the glew static library
  cd ../../

# (2) build GLFW      # https://github.com/glfw/glfw
# TODO: change to Curl or Wget or Forked Repo!
  cmake -S ./GLFW -B ./GLFW/build && make -C ./GLFW/build

# (3) get GLM
  git clone -b 0.9.9.8 https://github.com/g-truc/glm.git
  rm -rf glm/.git

# (4) build spdlog    # https://github.com/gabime/spdlog
# TODO: change to Curl or Wget or Forked Repo!
  git clone -b v1.11.0 https://github.com/gabime/spdlog.git
  rm -rf spdlog/.git
  cmake -S ./spdlog -B ./spdlog/build && make -C ./spdlog/build -j

# (5) build assimp    # https://github.com/assimp/assimp
# TODO: change to Curl or Wget or Forked Repo!
  git clone -b v5.2.5 https://github.com/assimp/assimp.git
  rm -rf assimp/.git
  cmake -S ./assimp -B ./assimp/build && make -C ./assimp/build -j

