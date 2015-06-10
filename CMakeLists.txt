project("ebusd")
cmake_minimum_required(VERSION 3.2)


set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -g -std=c++11 -D_WIN32_WINNT=0x600 -pthread -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -D_WIN32_WINNT=0x600 -pthread -pedantic")


################################################################################
# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.

#set(MYSQL_FIND_REQUIRED  TRUE)
#INCLUDE(FindMySqlLibs.cmake)








################################################################################

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES ("sys/param.h;sys/mount.h" HAVE_SYS_MOUNT_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config/config.h)



set(TODO_SOURCE_DIR    ${CMAKE_SOURCE_DIR}/source)
set(TODO_INCLUDE_DIR   ${CMAKE_SOURCE_DIR}/include
                       ${CMAKE_SOURCE_DIR}/external)
set(BOOST_INCLUDE_DIR  C:/Temp/CPP/extlibs/boost_1_58_0)

set(MYSQL_INCLUDE_DIR C:/Temp/CPP/extlibs/mysql-connector-c-6.1.6-src/include)


include_directories(${CMAKE_CURRENT_BINARY_DIR}/config
                    ${TODO_INCLUDE_DIR}
                    ${MYSQL_INCLUDE_DIR}
                    ${BOOST_INCLUDE_DIR}
                    )

aux_source_directory(${TODO_SOURCE_DIR} TODO_SOURCE)

add_executable(${PROJECT_NAME}  ${TODO_SOURCE})