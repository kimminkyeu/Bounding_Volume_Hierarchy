#**********************************************
#             QT OpenGL Settings              #
#**********************************************

# Run build_dependency.sh script before compilation (ex. GLEW, GLFW)
# ----------------------------------------------------------------------------
#ADD_CUSTOM_COMMAND(
#        TARGET ${NAME}
#        COMMAND ${CMAKE_SOURCE_DIR}/dependency/build_dependency.sh
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} # where CMakeLists.txt is located
#        COMMENT "${BoldGreen}Building project dependency binaries [GLEW, GLFW]...${ColourReset}"
#)

# CMAKE Setting
# -------------------------------------------------
cmake_minimum_required( VERSION 3.24 ) # set cmake version
set( CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE ) # close DEPRECATION WARNING...
set( CMAKE_CXX_STANDARD 20 ) # Cpp compiler version (if compiler doesn't support standard, this may change automatically thus warning or error will not appear)
set( CMAKE_CXX_STANDARD_REQUIRED ON ) #  make all targets to follow this declaration during compilation (if set to REQUIRED, then above option (CXX_STANDARD) will be forced, thus warning or error will appear.
#set( CMAKE_FIND_DEBUG_MODE ON ) # print directory of found packages (called by find_package())
set( NAME scoop )
set( DEBUG_MODE ON ) # NOTE: set DEBUG_MODE to OFF in release version
project( ${NAME} ) # set project name
include( ${CMAKE_SOURCE_DIR}/utils.cmake ) # custom print_message() function.

# Qt Setting
set( CMAKE_AUTOUIC ON ) # Qt User Interface Compiler # https://doc.qt.io/qt-6/uic.html
set( CMAKE_AUTOMOC ON ) # Qt Meta-Object Compiler # https://doc.qt.io/qt-6/moc.html
set( CMAKE_AUTORCC ON ) # Qt Resource Compiler # https://doc.qt.io/qt-6/resources.html

# TODO:  ***** SET QT INSTALLED LOCATION ***********************************
    set( CMAKE_PREFIX_PATH "~/Qt/6.5.2/gcc_64/lib/cmake") # linux
    # set( CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/dependency/QtBaseBinary/lib/cmake")
# **************************************************************************
find_package( QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets )
find_package( Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets )

# OpenGL setting
#set( OpenGL_GL_PREFERENCE LEGACY )
#find_package( OpenGL REQUIRED ) # stops processing with an error message if the package cannot be found.

# GLM setting
# https://github.com/g-truc/glm/blob/master/manual.md#-15-finding-glm-with-cmake
set( GLM_DIR ${CMAKE_SOURCE_DIR}/dependency/glm/cmake/glm ) # if necessary
find_package( GLM NAMES glm CONFIG REQUIRED )

# spdlog setting
# https://github.com/gabime/spdlog/blob/v1.x/example/CMakeLists.txt
set(SPDLOG_DIR ${CMAKE_SOURCE_DIR}/dependency/spdlog/build) # if necessary
find_package( SPDLOG NAMES spdlog CONFIG REQUIRED )

# Set Project Variable
set( SOURCE_FILES
    # QT Test
        Test/main.cpp
        Test/mainwindow.cpp
        Test/mainwindow.h
        Test/mainwindow.ui
    # Project
#        LunarApp/src/App.cpp
    # Lunar Core
#        Lunar/Core/Application.cpp
#        Lunar/Input/Input.cpp
#        Lunar/Shader/ShaderProgram.cpp
#        Lunar/Mesh/Mesh.cpp
#        Lunar/Camera/PlayerCamera.cpp # Player's view
#        Lunar/Camera/EditorCamera.cpp # Editor View
#        Lunar/Texture/Texture.cpp
#        Lunar/Model/Model.cpp
#        Lunar/Light/Light.cpp
#        Lunar/Material/Material.cpp
        )

# Project Setting
# ----------------------------------------------------------------------------
if ( ${QT_VERSION_MAJOR} GREATER_EQUAL 6 )
    qt_add_executable( ${NAME}
            MANUAL_FINALIZATION
            ${SOURCE_FILES}
    )
else ()
    add_executable( ${NAME} ${SOURCE_FILES} )
endif ()

# Set cmake to add define variable to specific target.
# ----------------------------------------------------------------------------
target_compile_definitions( ${NAME}
        PRIVATE
        DEBUG_MODE
        PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}" # 이 행위를 통해, 실행 파일 내부 에서 fopen등 함수 사용시 CMakelist.txt 파일 기준으로 경로 설정하기 위함.
        GLM_FORCE_SWIZZLE # force GLM to report the configuration as part of the build log
        )

# Link libraries (archive files)
# ----------------------------------------------------------------------------
target_link_libraries( ${NAME}
        PRIVATE # https://cmake.org/pipermail/cmake/2016-May/063400.html
#        OpenGL::GL # gl-native 함수들 링크 (Ex. glXGetProcAddressARB)
        Qt${QT_VERSION_MAJOR}::Widgets
#        ${CMAKE_SOURCE_DIR}/dependency/GLEW/build/lib/libGLEW.a # static lib
#        ${CMAKE_SOURCE_DIR}/dependency/GLFW/build/src/libglfw3.a # static lib
        glm::glm
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
        )

# TODO: Delete Later !! **************************************************
if(CMAKE_SYSTEM_NAME STREQUAL "Linux") # Linux
    target_link_libraries( ${NAME}
            PRIVATE
            ${CMAKE_SOURCE_DIR}/dependency/assimp/build/bin/libassimp.so
            )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # MacOS
    target_link_libraries( ${NAME}
            PRIVATE
            ${CMAKE_SOURCE_DIR}/dependency/assimp/build/bin/libassimp.5.2.5.dylib
            )
    # ... ?
endif ()
# TODO: ******************************************************************


# Set Include Path for Headers
# ----------------------------------------------------------------------------
# [AFTER]   : add prefix to the given directory
# [SYSTEM]  : #include <...>
include_directories( AFTER ${CMAKE_SOURCE_DIR} SYSTEM
#        dependency/GLEW/include
#        dependency/GLFW/include
        dependency/glm/glm
        dependency/stb_image
        dependency/spdlog/include
        dependency/assimp/include # TODO: Delete Later!
        dependency/assimp/build/include # TODO: Delete Later!
        )
include_directories( #include "..."
        ${CMAKE_SOURCE_DIR}
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

if( DEBUG_MODE )
    print_message(WARNING "------------------------------------------------------------------------------------")
    print_message(WARNING "You are building project in Debug mode. (using spdlog library)")
    print_message(WARNING "If you are building release version, then set DEBUG_MODE to False in CmakeList.txt")
    print_message(WARNING "------------------------------------------------------------------------------------")
endif ()

set_target_properties( ${NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        )

include(GNUInstallDirs)
install(TARGETS ${NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable( ${NAME} )
endif()
