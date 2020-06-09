#!/bin/bash

#install libs (boost, mariadb) and cmake
sudo apt-get -yq --no-install-suggests --no-install-recommends install libboost-all-dev libmariadb-dev cmake

rm -r build
mkdir build
cd build
cmake ..
make
cd ..
