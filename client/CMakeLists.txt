cmake_minimum_required(VERSION 3.27)
project(Client)

add_library(client ${CMAKE_CURRENT_LIST_DIR}/src/client.cpp)

set(BOOST /opt/homebrew/Cellar/boost/1.83.0/lib)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)


target_link_libraries(client PUBLIC 
nlohmann_json::nlohmann_json 
INTERFACE
OpenSSL::SSL
OpenSSL::Crypto
${BOOST}/libboost_system-mt.a 
${BOOST}/libboost_coroutine.a 
)

target_include_directories(client
PUBLIC
${CMAKE_SOURCE_DIR}/ext/websocketpp 
${CMAKE_CURRENT_LIST_DIR}/include
PRIVATE
${CMAKE_CURRENT_LIST_DIR}/private
)

