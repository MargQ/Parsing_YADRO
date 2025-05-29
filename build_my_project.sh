#!/bin/bash
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/l2_project ..
make
make install
