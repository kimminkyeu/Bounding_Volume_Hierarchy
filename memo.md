
1. GLEW 빌드. (하위 디렉토리) --> makefile
    ------------------------------------
    - https://github.com/nigels-com/glew

        make -C ./glew (=path to glew)



2. GLFW 빌드. --> cmake (cmake이 시스템에 따라 헤더파일을 자동생성)
    ------------------------------------
    - https://www.glfw.org/docs/latest/compile.html

        cmake -S {path/to/glfw} -B {path/to/build}
        make -C {path/to/makefile_created_by_cmake}
