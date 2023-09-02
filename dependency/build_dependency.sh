#!/bin/bash

# save absolute location of dependency directory
DEPENDENCY_DIR=$(pwd)

# prints colored text
print_style () {
    if [ "$2" == "info" ] ; then
        COLOR="96m";
    elif [ "$2" == "success" ] ; then
        COLOR="92m";
    elif [ "$2" == "warning" ] ; then
        COLOR="93m";
    elif [ "$2" == "danger" ] ; then
        COLOR="91m";
    else #default color
        COLOR="0m";
    fi
    STARTCOLOR="\e[$COLOR";
    ENDCOLOR="\e[0m";
    printf "$STARTCOLOR%b$ENDCOLOR" "$1";
}

print_dependency_header () {
  print_style "-------------------------------------------\n" "info";
  print_style "   Building $1 \n"                             "info";
  print_style "-------------------------------------------\n" "info";
}

print_directory_exist_warning () {
  print_style "-------------------------------------------\n" "warning";
  print_style "   $1 already exists. skipping... \n"          "warning";
  print_style "-------------------------------------------\n" "warning";
}

__print_build_finished__ () {
  print_style "-------------------------------------------\n" "success";
  print_style "   Build Finished! \n"                         "success";
  print_style "-------------------------------------------\n" "success";
}

dependencyNotInstalled () {
  # if directory exist + directory not empty
  if [ -d "$DEPENDENCY_DIR/$1" ] && [ "$(ls -A $DEPENDENCY_DIR/$1)" ]
  then
    false
  else
    true
  fi
}


# get ImGui       # https://github.com/ocornut/imgui/tree/docking
# -----------------------------------------------------
TARGET="imgui"
if dependencyNotInstalled $TARGET
then
  print_dependency_header $TARGET
  git clone -b docking https://github.com/ocornut/imgui.git $TARGET
  rm -rf $TARGET/.git
else
  print_directory_exist_warning $TARGET
fi

# get GLM
# -----------------------------------------------------
TARGET="glm"
if dependencyNotInstalled $TARGET
then
  print_dependency_header $TARGET
  git clone -b 0.9.9.8 https://github.com/g-truc/glm.git $TARGET
  rm -rf $TARGET/.git
else
  print_directory_exist_warning $TARGET
fi

# build spdlog    # https://github.com/gabime/spdlog
# -----------------------------------------------------
TARGET="spdlog"
if dependencyNotInstalled $TARGET
then
  print_dependency_header $TARGET
  git clone -b v1.11.0 https://github.com/gabime/spdlog.git $TARGET
  rm -rf $TARGET/.git
  cmake -S ./$TARGET -B ./$TARGET/build && make -C ./$TARGET/build -j
else
  print_directory_exist_warning $TARGET
fi

# build assimp    # https://github.com/assimp/assimp
# -----------------------------------------------------
TARGET="assimp"
if dependencyNotInstalled $TARGET
then
  print_dependency_header $TARGET
  git clone -b v5.2.5 https://github.com/assimp/assimp.git $TARGET
  rm -rf $TARGET/.git
  cmake -S ./$TARGET -B ./$TARGET/build && make -C ./$TARGET/build -j
else
  print_directory_exist_warning $TARGET
fi

# build GLEW      # https://github.com/nigels-com/glew
# -----------------------------------------------------
# TODO: change to Curl or Wget or Forked Repo!
TARGET="glew"
print_dependency_header $TARGET
cd ./GLEW/build
cmake ./cmake && make glew_s # build the glew static library
cd ../../

# build GLFW      # https://github.com/glfw/glfw
# -----------------------------------------------------
# TODO: change to Curl or Wget or Forked Repo!
TARGET="glfw"
print_dependency_header $TARGET
cmake -S ./GLFW -B ./GLFW/build && make -C ./GLFW/build


# ...
__print_build_finished__