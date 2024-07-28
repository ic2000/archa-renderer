cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "")

project(archa-renderer VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(dependencies)
add_subdirectory(src)

add_custom_target(
  link_resources ALL
  COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/resources"
          "${CMAKE_BINARY_DIR}/src/resources")

add_custom_target(
  generate_compile_commands ALL
  COMMAND compdb -p "${CMAKE_BINARY_DIR}" list >
          "${CMAKE_SOURCE_DIR}/compile_commands.json")
