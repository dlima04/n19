cmake_minimum_required(VERSION 3.28)

project(n19
  VERSION 0.0.0
  DESCRIPTION "The n19 Project"
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Build options
option(BUILD_TESTS "Build All Unit Tests" ON)
option(BUILD_RL "Build the Reference Language Executable" ON)
option(ENABLE_ASAN "Enable Clang Address Sanitizer" ON)
option(ENABLE_UBSAN "Enable Clang UB Sanitizer" OFF)
option(ALLOW_LOGGING "Allow logging features" OFF)

set(N19_BUILD_PLATFORM ${CMAKE_HOST_SYSTEM_NAME})
set(N19_IS_DARWIN OFF)
set(N19_IS_LINUX OFF)
set(N19_IS_POSIX OFF)
set(N19_IS_WINDOWS OFF)
set(N19_IS_CLANGCL_WINDOWS OFF)

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

if(N19_IS_WINDOWS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(N19_IS_CLANGCL_WINDOWS ON)
endif()

set(N19_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${N19_SOURCE_DIR}/Meta/CMake")

# include Meta/CMake modules
include(Common)
include(Sanitizers)

# Add project root as an include directory
include_directories(${N19_SOURCE_DIR})

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_23)

# Project-wide warnings
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wno-pedantic>
)

if(N19_IS_CLANGCL_WINDOWS)
  target_compile_options(project_warnings INTERFACE
    -Wno-c++98-compat-pedantic
    -Wno-newline-eof
    -Wno-reserved-macro-identifier
    -Wno-extra-semi
    -Wno-c++20-compat
  )
endif()

# Address sanitizer.
# Note: ignore this option if on Windows.
if(ENABLE_ASAN AND NOT N19_IS_WINDOWS)
  configure_clang_sanitizer(address)
  link_clang_sanitizer(address)
endif()

add_subdirectory(Core)
add_subdirectory(Framework)
add_subdirectory(System)

# Optional - build unit tests
if(BUILD_TESTS)
  add_subdirectory(Bulwark)
  add_subdirectory(Tests)
endif()

# Optional - build reference language
if(BUILD_RL)
  add_subdirectory(Frontend)
endif()
