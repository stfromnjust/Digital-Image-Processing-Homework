cmake_minimum_required(VERSION 3.19)
project(chap3)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(chap3
        bmpFile.h
        bmpFile.cpp
        main.cpp)