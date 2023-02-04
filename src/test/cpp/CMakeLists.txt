cmake_minimum_required(VERSION 3.14)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

# Download GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

# source files
file(GLOB_RECURSE TEST_SRC
  LIST_DIRECTORIES false
  *.h
  *_test.cpp
)

# project
set(TEST_BIN ${CMAKE_PROJECT_NAME}_test)
add_executable(${TEST_BIN} ${TEST_SRC} ${MAIN_SRC})

target_link_libraries(${TEST_BIN} gtest_main)

include(GoogleTest)
gtest_discover_tests(${TEST_BIN})
