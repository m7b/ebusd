cmake_minimum_required(VERSION 3.2)
project("ebusd")

OPTION(CI_BUILD "Build in CI" OFF)
OPTION(COVERAGE "Build with coverage" OFF)

IF(CI_BUILD)
  MESSAGE("-- CI_BUILD is ON")
ELSE()
  MESSAGE("-- CI_BUILD is OFF")
ENDIF()

IF(COVERAGE)
  MESSAGE("-- COVERAGE is ON")
ELSE()
  MESSAGE("-- COVERAGE is OFF")
ENDIF()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

IF(WIN32)
  set(MARIADB_INCLUDE_DIR C:/CPP/tools/msys64/mingw64/include)
  set(MARIADB_LIBRARY_DIR C:/CPP/tools/msys64/mingw64/lib)
ENDIF()
find_package(MariaDB REQUIRED)


# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)


# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config/config.h
)

  
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(
  ${CMAKE_SOURCE_DIR}/inc
  ${CMAKE_SOURCE_DIR}/lib/RS-232
  #MSYS2 libmariadbclient, libboost
  # pacman -S mingw64/mingw-w64-x86_64-libmariadbclient
  # pacman -S mingw64/mingw-w64-x86_64-boost
  ${MARIADB_INCLUDE_DIR}
  C:/CPP/tools/msys64/mingw64/include
)

# add link directories
link_directories(
  ${MARIADB_LIBRARY_DIR}
)

add_library(
  rs232
  ${CMAKE_SOURCE_DIR}/lib/RS-232/rs232.c
)

add_library(
  item
  src/item.cpp
)

# add the executable
add_executable(${CMAKE_PROJECT_NAME}
  src/ebus.cpp
  src/msg.cpp
  #src/serial.cpp
)

# set "gcc-ar" because of warning by compilling with flag -flto in windows gcc 8.1 compiler 
IF(WIN32)
  MESSAGE("-- Set CMAKE_AR to gcc-ar in WIN32 build because of flag -flto")
  SET(CMAKE_AR "gcc-ar")
ENDIF()


# target compile options
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:-g>)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:-O0>)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:-pedantic>)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC $<$<CONFIG:Release>:-O3>)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -std=c++11)  #-std=gnu++17
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -pthread)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -flto)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -Wall)
IF(WIN32)
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -static)
ENDIF()

IF(COVERAGE)
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:--coverage>)
ENDIF()


# target compile definitions
#-D_WIN32_WINNT=0x600
#target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC -D__GNUWIN32__)


# target link libraries
IF(WIN32)
  IF(COVERAGE)
    target_link_libraries(
      ${CMAKE_PROJECT_NAME}
      item
      rs232
      mariadbclient
      -lssl
      -lcrypto
      -lws2_32
      -lz
      -static
      $<$<CONFIG:Release>:-s>
      --coverage
    )
  ELSE()
    target_link_libraries(
      ${CMAKE_PROJECT_NAME}
      item
      rs232
      mariadbclient
      -lssl
      -lcrypto
      -lws2_32
      -lz
      -static
      $<$<CONFIG:Release>:-s>
    )
  ENDIF()
ELSE()
  IF(COVERAGE)
    target_link_libraries(
      ${CMAKE_PROJECT_NAME}
      item
      rs232
      mariadb
      -pthread
      $<$<CONFIG:Release>:-s>
      --coverage
    )
  ELSE()
    target_link_libraries(
      ${CMAKE_PROJECT_NAME}
      item
      rs232
      mariadb
      -pthread
      $<$<CONFIG:Release>:-s>
    )
  ENDIF()
ENDIF()

add_subdirectory(test)