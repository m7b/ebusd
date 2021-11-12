# ebus protocol logger

### Test results

Branches  | Linux | Windows | Test coverage | Security analysis
----------|-------|---------|---------------| -----------------
Master:   | [![example workflow](https://github.com/m7b/ebusd/actions/workflows/cmake.yml/badge.svg)](https://github.com/m7b/ebusd/actions?query=workflow%3ACmake) | [![Build status](https://ci.appveyor.com/api/projects/status/0ct4tlnm6rkxava0?svg=true)](https://ci.appveyor.com/project/m7b/ebusd) | [![Coverage Status](https://coveralls.io/repos/github/m7b/ebusd/badge.svg?branch=master)](https://coveralls.io/github/m7b/ebusd?branch=master) | [![Total alerts](https://img.shields.io/lgtm/alerts/g/m7b/ebusd.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/m7b/ebusd/alerts/)

### About

ebusd is a program to log ebus telegrams and write it to a MySQL/MariaDB database.

### Build instructions

```bash
mkdir build
cd build
cmake  ..
make
```

### Dependency

This application requires boost and mariadb library.
