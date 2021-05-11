cmake_minimum_required(VERSION 3.17)
project(chap1)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(chap1
        bmp.cpp
        bmp.h)
