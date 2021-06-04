cmake_minimum_required(VERSION 3.19)
project(chap4)

set(CMAKE_CXX_STANDARD 11)

add_executable(chap4
        bmpFile.h
        bmpFile.cpp
        main.cpp)

find_package(OpenCV REQUIRED)
target_link_libraries(chap4 ${OpenCV_LIBS})