cmake_minimum_required(VERSION 3.20)
project(chap6)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(chap6
        bmpFile.cpp
        bmpFile.h
        main.cpp)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMY_DEBUG")
