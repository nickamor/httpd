cmake_minimum_required(VERSION 3.6)
project(httpd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

set(SOURCE_FILES
        src/common.c
        src/config-file.c
        src/http.c
        src/list.c
        src/server-common.c
        src/sockets.c
        src/main.c)

add_executable(httpd ${SOURCE_FILES} src/server-single/main.c)

add_executable(httpd-threaded ${SOURCE_FILES} src/server-threaded/main.c)

add_executable(httpd-forked ${SOURCE_FILES} src/server-forked/main.c src/server-forked/childpid.c)

add_executable(httpd-select ${SOURCE_FILES} src/server-select/main.c)
