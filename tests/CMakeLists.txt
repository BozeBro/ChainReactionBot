cmake_minimum_required(VERSION 3.27)
project(Test)

add_executable(test test_chain.cpp)
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/include/)
