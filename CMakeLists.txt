
# https://cmake.org/cmake/help/latest/guide/tutorial/index.html

cmake_minimum_required(VERSION 3.24) # set cmake version

project(scoop) # set project name

#GLEW 라이브러리 설정 (static lib)
set(GLEW_LIBRARIES      libGLEW.a )
set(GLEW_INCLUDE_DIRS   ./GLEW/include )  # include <{path/to/include}> 를 가능케 하는 옵션.
set(GLEW_LIBRARY_DIRS   ./lib/GLEW/lib )
#find_package(GLEW REQUIRED)

#GLFW 라이브러리 설정 (static lib)
set(GLFW_LIBRARIES      libglfw3.a )
set(GLFW_INCLUDE_DIRS   ./GLFW/include ) # include <{path/to/include}> 를 가능케 하는 옵션.
set(GLFW_LIBRARY_DIRS   ./lib/GLFW/build/src )
#find_package(GLFW REQUIRED)

# https://stackoverflow.com/questions/19159599/cmake-set-target-properties-and-system-include-directories/19161129#19161129
include_directories(SYSTEM
        ${GLEW_INCLUE_DIRS}
        ${GLFW_INCLUE_DIRS}
        )

LINK_LIBRARIES(
        ${GLEW_LIBRARIES}
        ${GLFW_LIBRARIES}
        )

LINK_DIRECTORIES(
        ${GLEW_LIBRARY_PATH}
        ${GLFW_LIBRARY_PATH}
)

add_executable(scoop scoop.cpp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
