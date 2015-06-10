@ECHO OFF
REM SET CMAKE_C_COMPILER=C:/Program Files (x86)/CodeBlocks/nuwen-GCC-32_5.1/bin/gcc
REM SET CMAKE_CXX_COMPILER=C:/Program Files (x86)/CodeBlocks/nuwen-GCC-32_5.1/bin/g++

REM SET CC=C:/Program Files (x86)/CodeBlocks/nuwen-GCC-32_5.1/bin/gcc
REM SET CXX=C:/Program Files (x86)/CodeBlocks/nuwen-GCC-32_5.1/bin/g++

SET CC=C:/Program Files (x86)/CodeBlocks/TDM-GCC-64_4.9.2/bin/gcc
SET CXX=C:/Program Files (x86)/CodeBlocks/TDM-GCC-64_4.9.2/bin/g++

SET PATH=%PATH%;C:\Program Files (x86)\CodeBlocks\TDM-GCC-64_4.9.2\bin

mkdir build
cd build

REM cmake -G "CodeBlocks - Unix Makefiles" ..
cmake -G "Unix Makefiles" ..

make

cd ..