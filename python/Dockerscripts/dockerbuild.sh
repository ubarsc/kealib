#!/usr/bin/env bash

# Simple script that does the building in a conda-like environment
# I've copied all the env vars accross that conda uses

# Called buy buildpackage.sh

set -e

echo $USER
. /opt/conda/etc/profile.d/conda.sh
conda activate mybuild

export BUILD_PREFIX=$CONDA_PREFIX
export PREFIX=$CONDA_PREFIX
export AR=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-ar
export AS=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-as
export CXXFILT=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-c++filt
export ELFEDIT=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-elfedit
export GPROF=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-gprof
export LD_GOLD=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-ld.gold
export LD=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-ld
export NM=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-nm
export OBJCOPY=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-objcopy
export OBJDUMP=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-objdump
export RANLIB=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-ranlib
export READELF=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-readelf
export SIZE=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-size
export STRINGS=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-strings
export STRIP=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-strip
export CC=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-cc
export CFLAGS="-march=nocona -mtune=haswell -ftree-vectorize -fPIC -fstack-protector-strong -fno-plt -O2 -ffunction-sections -pipe -isystem $PREFIX/include -fdebug-prefix-map=$SRC_DIR=/usr/local/src/conda/qgis-3.10.13 -fdebug-prefix-map=$PREFIX=/usr/local/src/conda-prefix"
export CMAKE_ARGS="-DCMAKE_LINKER=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-ld -DCMAKE_STRIP=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-strip -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY -DCMAKE_FIND_ROOT_PATH=$PREFIX;$BUILD_PREFIX/x86_64-conda-linux-gnu/sysroot -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_INSTALL_LIBDIR=lib"
export CMAKE_PREFIX_PATH=$PREFIX:$BUILD_PREFIX/x86_64-conda-linux-gnu/sysroot/usr
export CONDA_BUILD_SYSROOT=$BUILD_PREFIX/x86_64-conda-linux-gnu/sysroot
export _CONDA_PYTHON_SYSCONFIGDATA_NAME=_sysconfigdata_x86_64_conda_cos6_linux_gnu
export CPPFLAGS="-DNDEBUG -D_FORTIFY_SOURCE=2 -O2 -isystem $PREFIX/include"
export CPP=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-cpp
export DEBUG_CFLAGS="-march=nocona -mtune=haswell -ftree-vectorize -fPIC -fstack-protector-all -fno-plt -Og -g -Wall -Wextra -fvar-tracking-assignments -ffunction-sections -pipe -isystem $PREFIX/include -fdebug-prefix-map=$SRC_DIR=/usr/local/src/conda/qgis-3.10.13 -fdebug-prefix-map=$PREFIX=/usr/local/src/conda-prefix"
export DEBUG_CPPFLAGS="-D_DEBUG -D_FORTIFY_SOURCE=2 -Og -isystem $BUILD_PREFIX/include"
export GCC_AR=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-gcc-ar
export GCC=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-gcc
export GCC_NM=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-gcc-nm
export GCC_RANLIB=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-gcc-ranlib
export host_alias=x86_64-conda-linux-gnu
export HOST=x86_64-conda-linux-gnu
export LDFLAGS="-Wl,-O2 -Wl,--sort-common -Wl,--as-needed -Wl,-z,relro -Wl,-z,now -Wl,--disable-new-dtags -Wl,--gc-sections -Wl,-rpath,$PREFIX/lib -Wl,-rpath-link,$PREFIX/lib -L$PREFIX/lib"
export CXXFLAGS="-fvisibility-inlines-hidden -std=c++17 -fmessage-length=0 -march=nocona -mtune=haswell -ftree-vectorize -fPIC -fstack-protector-strong -fno-plt -O2 -ffunction-sections -pipe -isystem $PREFIX/include -fdebug-prefix-map=$SRC_DIR=/usr/local/src/conda/qgis-3.10.13 -fdebug-prefix-map=$PREFIX=/usr/local/src/conda-prefix"
export CXX_FOR_BUILD=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-c++
export CXX=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-c++
export DEBUG_CXXFLAGS="-fvisibility-inlines-hidden -std=c++17 -fmessage-length=0 -march=nocona -mtune=haswell -ftree-vectorize -fPIC -fstack-protector-all -fno-plt -Og -g -Wall -Wextra -fvar-tracking-assignments -ffunction-sections -pipe -isystem $PREFIX/include -fdebug-prefix-map=$SRC_DIR=/usr/local/src/conda/qgis-3.10.13 -fdebug-prefix-map=$PREFIX=/usr/local/src/conda-prefix"
export GXX=$BUILD_PREFIX/bin/x86_64-conda-linux-gnu-g++

cd /home/conda/recipe_root/python
rm -rf build
mkdir build
cd build

cmake -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX -D CMAKE_BUILD_TYPE=Release ..
make
make install
