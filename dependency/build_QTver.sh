#!/bin/bash

# save absolute location of dependency directory
DEPENDENCY_DIR=$(pwd)

# (1) Get Qtbase6 from source + remove .git
git clone -b 6.5.2 https://github.com/qt/qtbase.git QtBaseSource
cd QtBaseSource
rm -rf .git

# (2) Build Binary
mkdir qt6-build
cd qt6-build
../configure -prefix $DEPENDENCY_DIR/QtBaseBinary
cmake --build . --parallel
cmake --install .


# (3) build spdlog    # https://github.com/gabime/spdlog
cmake -S ./spdlog -B ./spdlog/build && make -C ./spdlog/build -j

# (4) build assimp    # https://github.com/assimp/assimp
cmake -S ./assimp -B ./assimp/build && make -C ./assimp/build -j

