cmake_minimum_required(VERSION 3.20)
project(chap5)

set(CMAKE_CXX_STANDARD 14)

add_executable(chap5
        main.cpp bmpFile.h bmpFile.cpp)

find_package(OpenCV REQUIRED)
target_link_libraries(chap5 ${OpenCV_LIBS} )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMY_DEBUG")
