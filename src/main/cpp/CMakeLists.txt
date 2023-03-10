cmake_minimum_required(VERSION 3.14)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# options
option(BUILD_TESTS "Build test programs" OFF)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("Compiler: ${CMAKE_CXX_COMPILER}")

# source files
file(GLOB_RECURSE MAIN_SRC
  ds/*.[ch]pp
  modular/*.[ch]pp
  readwrite/*.[ch]pp
  util/*.[ch]pp
)

file(GLOB BENCH_SRC modular-bench.cpp)

include_directories(
  .
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=leak")
endif ()

# project
project(modular)
set(CMAKE_CXX_FLAGS "-Wall -funroll-loops -fno-stack-limit -O3")

# build options
if (NOT DEFINED PROFILE_ON)
  set(PROFILE_ON false)
endif ()

if (NOT DEFINED TRACE_ON)
  set(TRACE_ON false)
endif ()

add_compile_definitions(PROFILE_ON=${PROFILE_ON})
add_compile_definitions(TRACE_ON=${TRACE_ON})

# tests with GoogleTest
if (BUILD_TESTS)
  add_subdirectory(../../test/cpp ../test)
else ()
  add_executable(modular-bench ${BENCH_SRC} ${MAIN_SRC})
endif ()
