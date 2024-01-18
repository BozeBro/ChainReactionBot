cmake_minimum_required(VERSION 3.27)
project(Monte)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(BOOST /opt/homebrew/Cellar/boost/1.83.0/lib)
# set(OPENSSL /opt/homebrew/Cellar/openssl@3/3.2.0_1/lib/libssl.3.dylib)
# /opt/homebrew/Cellar/boost/1.83.0/include/
add_executable(main main.cpp src/client.cpp src/montecarlo.cpp)
add_executable(test test.cpp src/montecarlo.cpp)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/ext/websocketpp include/)
target_include_directories(test PRIVATE include/)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(main PRIVATE ${BOOST}/libboost_system-mt.a ${BOOST}/libboost_coroutine.a ${OPENSSL} nlohmann_json::nlohmann_json OpenSSL::SSL)
target_compile_options(main BEFORE PRIVATE --std=c++17)
target_compile_definitions(main PRIVATE DEBUG=1)

target_compile_options(test BEFORE PRIVATE --std=c++17)

# set_property(TARGET main PROPERTY CXX_STANDARD 17)
