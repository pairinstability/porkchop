#!/bin/sh

# build dependencies 

#git submodule init
git submodule update

cd third_party/pykep
 git apply patch ../../fix-cmake-build-error-boost_find_component.patch
mkdir build
cd build
cmake ../
make -j8 && sudo make install
