# ebus protocol logger

### About

ubusd is a program to log ebus telegrams and write it to a database.

### Test results

Branches  | Linux | Windows |
----------|-------|---------|
Master:   | [![Build Status](https://travis-ci.org/m7b/ebusd.svg?branch=master)](https://travis-ci.org/m7b/ebusd) | [![Build status](https://ci.appveyor.com/api/projects/status/0ct4tlnm6rkxava0?svg=true)](https://ci.appveyor.com/project/m7b/ebusd) |


### Build instructions

```bash
mkdir build
cd build
cmake  ..
make
```

### Dependency

This application requires boost and mariadb library.