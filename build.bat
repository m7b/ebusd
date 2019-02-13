@ECHO OFF

SET PATH=%PATH%;C:\CPP\tools\cmake-3.13.3-win64-x64\bin
SET PATH=%PATH%;C:\CPP\compiler\i686-8.1.0-release-posix-dwarf-rt_v6-rev0\mingw32\bin

mkdir build
cd build

REM cmake -G "CodeBlocks - MinGW Makefiles" ..
cmake -G "MinGW Makefiles" ..

cmake --build .
REM cmake --build . -- VERBOSE=1

cd ..