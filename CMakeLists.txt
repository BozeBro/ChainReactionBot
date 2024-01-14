cmake_minimum_required(VERSION 3.27)
project(Monte)
# include_directories("/opt/homebrew/include/")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(BOOST /opt/homebrew/Cellar/boost/1.83.0/lib)
include_directories(/opt/homebrew/Cellar/boost/1.83.0/include/)
include_directories(${CMAKE_SOURCE_DIR}/ext/websocketpp)
# find_package(websocketpp CONFIG REQUIRED)
# install(TARGETS ${CMAKE_CURRENT_BINARY_DIR}/ext/websocketpp/cmake_install.cmake)
add_executable(Monte main.cpp)

target_link_libraries(Monte PUBLIC ${BOOST}/libboost_system-mt.a ${BOOST}/libboost_coroutine.a PRIVATE nlohmann_json::nlohmann_json)
set_property(TARGET Monte PROPERTY CXX_STANDARD 17)
find_package(nlohmann_json 3.2.0 REQUIRED)

