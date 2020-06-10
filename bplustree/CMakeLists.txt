cmake_minimum_required(VERSION 3.13)
project(bplustree)

set(CMAKE_CXX_STANDARD 14)
add_executable(bplustree bplustree.cpp mytest.cpp)
include_directories(../common)

