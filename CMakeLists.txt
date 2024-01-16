cmake_minimum_required(VERSION 3.27)
project(Monte)
# include_directories("/opt/homebrew/include/")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(BOOST /opt/homebrew/Cellar/boost/1.83.0/lib)
# set(OPENSSL /opt/homebrew/Cellar/openssl@3/3.2.0_1/lib/libssl.3.dylib)
# /opt/homebrew/Cellar/boost/1.83.0/include/
include_directories(${CMAKE_SOURCE_DIR}/ext/websocketpp include/)

# install(TARGETS ${CMAKE_CURRENT_BINARY_DIR}/ext/websocketpp/cmake_install.cmake)
add_executable(Monte main.cpp src/client.cpp)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(Monte PRIVATE ${BOOST}/libboost_system-mt.a ${BOOST}/libboost_coroutine.a ${OPENSSL} nlohmann_json::nlohmann_json OpenSSL::SSL)
set_property(TARGET Monte PROPERTY CXX_STANDARD 17)
target_compile_definitions(Monte PRIVATE DEBUG=1)
