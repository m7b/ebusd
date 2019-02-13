#!/bin/bash

#install gcc-8 and libs (boost, mariadb) and cmake, git
sudo apt-get -yq --no-install-suggests --no-install-recommends install gcc-8 g++-8 libboost-all-dev libmariadb-client-lgpl-dev git cmake

export CC=gcc-8
export CXX=g++-8

rm -r build
mkdir build
cd build
cmake ..
make
cd ..