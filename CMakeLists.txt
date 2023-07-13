
# https://cmake.org/cmake/help/latest/guide/tutorial/index.html
cmake_minimum_required(VERSION 3.24) # set cmake version

# set cmake vairable
set(NAME scoop)
set(CMAKE_CXX_STANDARD 20) # Cpp compiler version
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(${NAME}) # set project name


# GLEW 라이브러리 설정 (static lib)

# set(GLEW_LIBRARIES      libGLEW.a )
# set(GLEW_INCLUDE_DIRS   ./GLEW/include )  # include <{path/to/include}> 를 가능케 하는 옵션.
# set(GLEW_LIBRARY_DIRS   /lib/GLEW/lib )

# GLFW 라이브러리 설정 (static lib)

# set(GLFW_LIBRARIES      libglfw3.a )
# set(GLFW_INCLUDE_DIRS   ./GLFW/include/ ) # include <{path/to/include}> 를 가능케 하는 옵션.
# set(GLFW_LIBRARY_DIRS   /lib/GLFW/build/src )

# https://stackoverflow.com/questions/19159599/cmake-set-target-properties-and-system-include-directories/19161129#19161129

# include_directories( SYSTEM
#         ${GLEW_INCLUE_DIRS}
#         ${GLFW_INCLUE_DIRS}
#         )

# include path
include_directories( SYSTEM # to use '#include <...>'
        ${CMAKE_SOURCE_DIR}/GLEW/include
        ${CMAKE_SOURCE_DIR}/GLFW/include
        )

add_executable(${NAME} scoop.cpp)

# link library (archive files)
target_link_libraries(${NAME}
        PRIVATE # ?
        ${CMAKE_SOURCE_DIR}/GLEW/build/lib/libGLEW.a
        ${CMAKE_SOURCE_DIR}/GLFW/build/src/libglfw3.a
        )



# Add custom rule (ex make fclean, re)
# ------------------------------------------------
ADD_CUSTOM_TARGET( re
        COMMENT "Re-building..."
        COMMAND make fclean && make all
        # ...
        )

ADD_CUSTOM_TARGET( fclean
        COMMENT "Cleaning executable and binaries..."
        COMMAND make clean && rm -rf 
        )
