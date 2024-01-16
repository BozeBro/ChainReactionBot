cmake_minimum_required(VERSION 3.27)
project(Monte)
# include_directories("/opt/homebrew/include/")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(BOOST /opt/homebrew/Cellar/boost/1.83.0/lib)
# set(OPENSSL /opt/homebrew/Cellar/openssl@3/3.2.0_1/lib/libssl.3.dylib)
# /opt/homebrew/Cellar/boost/1.83.0/include/
include_directories(${CMAKE_SOURCE_DIR}/ext/websocketpp include/)

add_executable(main main.cpp src/client.cpp)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(main PRIVATE ${BOOST}/libboost_system-mt.a ${BOOST}/libboost_coroutine.a ${OPENSSL} nlohmann_json::nlohmann_json OpenSSL::SSL)
set_property(TARGET main PROPERTY CXX_STANDARD 17)
target_compile_definitions(main PRIVATE DEBUG=1)
