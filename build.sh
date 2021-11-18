cd ../
rm -rf build
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=../install ../xdb
make
