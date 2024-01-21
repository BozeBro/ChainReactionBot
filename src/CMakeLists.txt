cmake_minimum_required(VERSION 3.27)
project(chain)

  add_library(chain ChainReaction.cpp)
  add_library(client client.cpp)
  target_include_directories(chain PRIVATE ../include)


  set(BOOST /opt/homebrew/Cellar/boost/1.83.0/lib)
  target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/ext/websocketpp ${CMAKE_SOURCE_DIR}/include/)

  find_package(nlohmann_json 3.2.0 REQUIRED)
  find_package(OpenSSL REQUIRED)
  target_link_libraries(client PRIVATE ${BOOST}/libboost_system-mt.a ${BOOST}/libboost_coroutine.a ${OPENSSL} nlohmann_json::nlohmann_json OpenSSL::SSL)
