cmake_minimum_required(VERSION 3.23)
project(Logger)

set(CMAKE_CXX_STANDARD 20)

#add_executable(Logger main.cpp )
add_executable(Logger
        main.cpp
        Utility/Logger.cpp Utility/Logger.h)
