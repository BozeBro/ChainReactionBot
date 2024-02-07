cmake_minimum_required(VERSION 3.27)
project(Bot)
add_library(bot ${CMAKE_CURRENT_LIST_DIR}/src/ChainReaction.cpp)

target_include_directories(bot PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
