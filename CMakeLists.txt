cmake_minimum_required(VERSION 3.28)

project(n19
  VERSION 0.0.0
  DESCRIPTION "The n19 Project"
  LANGUAGES C CXX
)

set(N19_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${N19_SOURCE_DIR}/Meta/CMake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(N19_BUILD_PLATFORM ${CMAKE_HOST_SYSTEM_NAME})
set(N19_IS_DARWIN OFF)
set(N19_IS_LINUX OFF)
set(N19_IS_POSIX OFF)
set(N19_IS_WINDOWS OFF)

if(WIN32 OR N19_BUILD_PLATFORM STREQUAL "Windows")
    set(N19_IS_WINDOWS ON)
elseif(N19_BUILD_PLATFORM STREQUAL "Linux")
    set(N19_IS_LINUX ON)
    set(N19_IS_POSIX ON)
elseif(N19_BUILD_PLATFORM STREQUAL "Darwin")
    set(N19_IS_DARWIN ON)
    set(N19_IS_POSIX ON)
else()
    message(FATAL_ERROR "Unsupported build platform.")
endif()

# Build options
option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_ASAN "Enable Clang Address Sanitizer" ON)
option(ALLOW_LOGGING "Allow logging features" OFF)

# Add project root as an include directory
include_directories(${N19_SOURCE_DIR})

# Project-wide compile options
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_23)

# Project-wide warnings
add_library(project_warnings INTERFACE)
target_compile_features(project_warnings INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
)

