cmake_minimum_required(VERSION 3.27)
project(Monte)


set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
if (prod)
  add_compile_definitions(PROD)
else()
  add_compile_definitions(DEBUG)
endif()

if (Test)
  add_subdirectory(src)
  add_subdirectory(tests)
  add_dependencies(test chain)
  target_link_libraries(test PRIVATE chain)
else()
  add_executable(main test.cpp)
  target_include_directories(main PRIVATE include/)

  add_subdirectory(src)
  add_dependencies(main chain)

  target_link_libraries(main chain)
endif()


