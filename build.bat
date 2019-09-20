@ECHO OFF

SET PATH=%PATH%;C:\CPP\tools\cmake-3.15.2-win64-x64\bin
SET PATH=%PATH%;C:\Program Files\CMake\bin
SET PATH=%PATH%;C:\CPP\compiler\x86_64-8.1.0-release-posix-seh-rt_v6-rev0\mingw64\bin

REM Get MSYS2 libraries (mariadb, boost)
CALL C:\CPP\tools\msys64\usr\bin\bash -l -c "pacman -Sy --noconfirm --needed mingw64/mingw-w64-x86_64-libmariadbclient mingw64/mingw-w64-x86_64-boost"

mkdir build
cd build

REM cmake -G "CodeBlocks - MinGW Makefiles" -DCI_BUILD:BOOL=OFF -DCOVERAGE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug ..
REM cmake -G "MinGW Makefiles" -DCI_BUILD:BOOL=OFF ..
cmake -G "MinGW Makefiles" -DCI_BUILD:BOOL=OFF -DCOVERAGE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug ..

cmake --build .
REM cmake --build . -- VERBOSE=1

cd test
ctest -V

cd ..\..