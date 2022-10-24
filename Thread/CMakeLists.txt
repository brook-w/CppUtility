cmake_minimum_required(VERSION 3.23)
project(Thread)

set(CMAKE_CXX_STANDARD 20)

add_executable(Thread main.cpp Utility/ThreadPoll.h)
