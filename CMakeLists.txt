
# set cmake version
# -------------------------------------------------
# https://cmake.org/cmake/help/latest/guide/tutorial/index.html
cmake_minimum_required(VERSION 3.24) # set cmake version

# let the preprocessor know about the system name
# ----------------------------------------------------------------------------
# Link: https://coderefinery.github.io/cmake-workshop/environment/
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # ... ?
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # ... ?
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # ... ?
else()
    # ... ?
endif()

# set cmake vairable
# ----------------------------------------------------------------------------
set(NAME scoop)
set(CMAKE_CXX_STANDARD 20) # Cpp compiler version
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SOURCE_FILES
        src/scoop.cpp
    )

# configure cmake project
# ----------------------------------------------------------------------------
project( ${NAME} ) # set project name
add_executable( ${NAME} ${SOURCE_FILES} )

# OPENGL
# ----------------------------------------------------------------------------
# 시스템에서 패키지를 찾고, 그 위치와 바이너리 이름들을 변수로 자동 저장해준다.
find_package( OpenGL REQUIRED ) # stops processing with an error message if the package cannot be found.

# include path
# ----------------------------------------------------------------------------
include_directories( SYSTEM # to use '#include <...>'
        ${CMAKE_SOURCE_DIR}/dependency/GLEW/include
        ${CMAKE_SOURCE_DIR}/dependency/GLFW/include
        )

# link library (archive files)
# ----------------------------------------------------------------------------
target_link_libraries( ${NAME}
        PRIVATE # --> [?] : 참고 https://cmake.org/pipermail/cmake/2016-May/063400.html
        OpenGL::GL # 이렇게 해야 gl-native 함수 (Ex. glXGetProcAddressARB)를 링크할 수 있다.
        # -lGL --> (find_package 명령어와 OpenGL::GL이 대신함)
        ${CMAKE_SOURCE_DIR}/dependency/GLEW/build/lib/libGLEW.a
        ${CMAKE_SOURCE_DIR}/dependency/GLFW/build/src/libglfw3.a
        -lX11
        )

# Add custom rule (ex make fclean, re)
# ----------------------------------------------------------------------------
ADD_CUSTOM_TARGET( re
        COMMENT "Re-building..."
        COMMAND make fclean && make all
        # ...
        )

ADD_CUSTOM_TARGET( fclean
        COMMENT "Cleaning executable and binaries..."
        COMMAND make clean # cmake이 생성하는 clean은 fclean과 동일.
        )
