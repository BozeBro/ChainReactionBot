cmake_minimum_required(VERSION 3.27)
project(ChainReaction)

add_library(chain ${CMAKE_CURRENT_LIST_DIR}/src/chainreaction.cpp)
target_include_directories(chain PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)