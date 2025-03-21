cmake_minimum_required(VERSION 3.28)
project(n19)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(N19_ENUMERATE_GLOBAL_SOURCES
  Frontend/ErrorCollector.cpp
  Frontend/EntityTable.cpp
  Frontend/Entity.cpp
  Frontend/EntityQualifier.cpp
  Frontend/DumpAst.cpp
  Frontend/Lexer.cpp
  Frontend/Token.cpp
  Sys/Error.cpp
  Sys/IODevice.cpp
  Sys/Time.cpp
  Sys/BackTrace.cpp
  Core/Panic.cpp
  Core/ArgParse.cpp
  IO/Console.cpp
  IO/Stream.cpp
  Sys/File.cpp
)

set(N19_ENUMERATE_GLOBAL_HEADERS
  Sys/String.hpp
  Sys/Error.hpp
  Sys/IODevice.hpp
  Sys/Handle.hpp
  Sys/Time.hpp
  Sys/BackTrace.hpp
  Frontend/Token.hpp
  Frontend/ErrorCollector.hpp
  Frontend/AstNodes.hpp
  Frontend/EntityQualifier.hpp
  Frontend/EntityTable.hpp
  Frontend/Entity.hpp
  Frontend/Lexer.hpp
  Misc/Global.hpp
  Misc/Macros.hpp
  Core/Panic.hpp
  Core/Concepts.hpp
  Core/RingBase.hpp
  Core/Tuple.hpp
  Core/Bytes.hpp
  Core/Platform.hpp
  Core/Iterator.hpp
  Core/ClassTraits.hpp
  Core/RingBuffer.hpp
  Core/RingQueue.hpp
  Core/Murmur3.hpp
  Core/TypeTraits.hpp
  Core/Defer.hpp
  Core/Result.hpp
  Core/Maybe.hpp
  Core/Nothing.hpp
  Core/Try.hpp
  Core/ArgParse.hpp
  IO/Console.hpp
  IO/Fmt.hpp
  IO/Stream.hpp
  Sys/File.hpp
  Frontend/Keywords.hpp
)

set(N19_ENUMERATE_PRIMARY_EXECUTABLES
  n19      # Main compiler executable
  bulwark  # The unit test executable
)

set(N19_BUILD_PLATFORM ${CMAKE_HOST_SYSTEM_NAME})
set(N19_IS_MACOS OFF)
set(N19_IS_LINUX OFF)
set(N19_IS_POSIX OFF)
set(N19_IS_WINDOWS OFF)

# Determine build platform...
if(WIN32 OR N19_BUILD_PLATFORM STREQUAL "Windows")
  set(N19_IS_WINDOWS ON)
elseif(N19_BUILD_PLATFORM STREQUAL "Linux")
  set(N19_IS_LINUX ON)
  set(N19_IS_POSIX ON)
elseif(N19_BUILD_PLATFORM STREQUAL "Darwin")
  set(N19_IS_MACOS ON)
  set(N19_IS_POSIX ON)
else()
  message(FATAL_ERROR "Unsupported build platform.")
endif()

# Build the main compiler executable
add_executable(n19
  Misc/CompilerMain.cpp
  ${N19_ENUMERATE_GLOBAL_SOURCES}
  ${N19_ENUMERATE_GLOBAL_HEADERS}
)

# Build the unit test executable
add_executable(bulwark
  Misc/BulwarkMain.cpp
  Bulwark/Reporting.cpp
  Bulwark/Registry.cpp
  Bulwark/Case.cpp
  Bulwark/Suite.cpp
  Bulwark/Case.hpp
  Bulwark/Registry.hpp
  Bulwark/BulwarkContext.hpp
  Bulwark/Suite.hpp
  Bulwark/Reporting.hpp
  Bulwark/Bulwark.hpp
  Bulwark/Suites/Core/SuiteMaybe.cpp
  Bulwark/Suites/Core/SuiteResult.cpp
  ${N19_ENUMERATE_GLOBAL_SOURCES}
  ${N19_ENUMERATE_GLOBAL_HEADERS}
)

function(add_platform_macros target)
  if(N19_IS_LINUX)
    target_compile_definitions(${target} PRIVATE N19_POSIX)
    target_compile_definitions(${target} PRIVATE N19_LINUX)
  elseif(N19_IS_MACOS)
    target_compile_definitions(${target} PRIVATE N19_POSIX)
    target_compile_definitions(${target} PRIVATE N19_MACOS)
  else()
    target_compile_definitions(${target} PRIVATE N19_WIN32)
    target_compile_definitions(${target} PRIVATE N19_DOGSHIT_PLATFORM)
  endif()
endfunction()

function(enable_clang_asan target)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Build Target ${target} -- Clang Asan Enabled")
    target_compile_options(${target} PRIVATE -fsanitize=address -g -fno-omit-frame-pointer)
    target_link_libraries(${target} PRIVATE -fsanitize=address)
  else()
    message(FATAL_ERROR "enable_clang_asan must be used on clang targets!")
    message(FATAL_ERROR "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
  endif()
endfunction()

# Build all executables
foreach(executable ${N19_ENUMERATE_PRIMARY_EXECUTABLES})
  target_include_directories(${executable} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_platform_macros(${executable})

  # TODO: this is temporary.
  enable_clang_asan(${executable})

  if(MSVC)
    target_compile_options(${executable} PRIVATE /FI Misc/Global.hpp)
  else()
    target_compile_options(${executable} PRIVATE -include Misc/Global.hpp)
  endif()
endforeach()
