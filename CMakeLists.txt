cmake_minimum_required(VERSION 3.6)
project(httpd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

set(SOURCE_FILES
    src/server-single/main.c
    src/common.c
    src/config-file.c
    src/http.c
    src/server-common.c
    src/sockets.c)

add_executable(httpd ${SOURCE_FILES})
