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

# specify compilers on local machine (Mac)
if (EXISTS /opt/homebrew/bin/g++-12)
  set(CMAKE_C_COMPILER /opt/homebrew/bin/gcc-12)
  set(CMAKE_CXX_COMPILER /opt/homebrew/bin/g++-12)
elseif(EXISTS /opt/homebrew/bin/gcc-11)
  set(CMAKE_C_COMPILER /opt/homebrew/bin/gcc-11)
  set(CMAKE_CXX_COMPILER /opt/homebrew/bin/g++-11)
endif ()

message("Compiler: ${CMAKE_CXX_COMPILER}")

# source files
file(GLOB_RECURSE MAIN_SRC
  tww/ds/set/*.[ch]pp
  tww/util/*.[ch]pp
)

include_directories(
  .
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=leak")
endif ()

# project
project(tww)
set(CMAKE_CXX_FLAGS "-Wall -funroll-loops -fno-stack-limit -O3")

# tests with GoogleTest
if (BUILD_TESTS)
  add_subdirectory(../../test/cpp ../test)
else ()
  # add_executable(bench ${BENCH_SRC} ${MAIN_SRC})
endif ()
