cmake_minimum_required(VERSION 3.27)
project(Monte)


set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

if (secure)
  add_compile_definitions(SECURE)
else()
  add_compile_definitions(INSECURE)
endif()

if (Test)
  add_subdirectory(src)
  add_subdirectory(tests)
  add_dependencies(test chain)
  target_link_libraries(test PRIVATE chain)
else()
  add_subdirectory(client)
  # add_executable(main test.cpp)
  add_executable(main main.cpp)

  add_subdirectory(bot)
  add_dependencies(main bot client )

  target_link_libraries(main PRIVATE bot client)

  # add_dependencies(cli client)
  # target_link_libraries(cli PRIVATE client)
endif()
