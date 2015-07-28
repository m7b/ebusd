@ECHO OFF

REM Toolchains
REM ==========
REM nuwen-GCC-32_5.1
REM TDM-GCC-32_4.4.1
REM TDM-GCC-32_4.8.1
REM TDM-GCC-64_4.9.2
REM TDM-GCC-32_5.1.0
REM TDM-GCC-64_5.1.0

SET TOOLCHAIN=TDM-GCC-64_5.1.0

SET CC=C:/Program Files (x86)/CodeBlocks/%TOOLCHAIN%/bin/gcc
SET CXX=C:/Program Files (x86)/CodeBlocks/%TOOLCHAIN%/bin/g++
SET PATH=%PATH%;C:\Program Files (x86)\CodeBlocks\%TOOLCHAIN%\bin

mkdir build
cd build

REM cmake -G "CodeBlocks - Unix Makefiles" ..
cmake -G "Unix Makefiles" ..

make VERBOSE=1

cd ..