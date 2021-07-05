
mkdir -p build
cd build

cmake -D CMAKE_INSTALL_PREFIX=$PREFIX \
    -D CMAKE_BUILD_TYPE=Release \
	-D CMAKE_PREFIX_PATH=$PREFIX \
	-D Python3_EXECUTABLE=$PYTHON \
	..
make
make install
