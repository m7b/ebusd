# ebus protocol logger

### Test results

Branches  | Linux | Windows | Test coverage |
----------|-------|---------|---------------|
Master:   | [![Build Status](https://travis-ci.org/m7b/ebusd.svg?branch=master)](https://travis-ci.org/m7b/ebusd) | [![Build status](https://ci.appveyor.com/api/projects/status/0ct4tlnm6rkxava0?svg=true)](https://ci.appveyor.com/project/m7b/ebusd) | [![Coverage Status](https://coveralls.io/repos/github/m7b/ebusd/badge.svg?branch=master)](https://coveralls.io/github/m7b/ebusd?branch=master) |

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