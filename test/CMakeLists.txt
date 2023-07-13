cmake_minimum_required(VERSION 3.24)
project(test)

include_directories( SYSTEM
        ./lib/include
        )

add_executable(test test.cpp)

target_link_libraries(test ${CMAKE_SOURCE_DIR}/lib/lib_test.a)
