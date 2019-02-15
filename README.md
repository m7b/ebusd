| [![Build Status](https://travis-ci.org/m7b/ebusd.svg?branch=master)](https://travis-ci.org/m7b/ebusd) | [![Build status](https://ci.appveyor.com/api/projects/status/0ct4tlnm6rkxava0?svg=true)](https://ci.appveyor.com/project/m7b/ebusd) |


Build instructions
==================

mkdir build
cd build
cmake -G "Unix Makefiles" ..
make


MySQL DLL conversion from MSVC to GCC
-------------------------------------

The following steps are from:
http://www.openwebspider.org/documentation/older-versions/how-to-link-libmysqllib-with-dev-c-or-gcc-under-windows/

- Download mysql-connector-c-6.1.6-winx64.zip from MySQL download server
- extract archive
- reimp.exe <FULL_PATH_OF(MySQL\lib)>libmysql.lib
  - This command creates: libmysql.def AND libmysql.a
  - tool could found here: http://prdownloads.sourceforge.net/mingw/mingw-utils-0.2.tar.gz?download
- after compilling libmysql.dll has to be copied beside the executable ebusd.exe
