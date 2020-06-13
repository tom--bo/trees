cmake_minimum_required(VERSION 3.13)
project(trees)
set(CMAKE_CXX_STANDARD 14)

include_directories(common)
include_directories(btree)
include_directories(bplustree)
include_directories(bastertree)

add_executable(trees bplustree/bplustree.cpp bplustree/bplustree.cpp btree/btree.cpp main.cpp)
