cmake_minimum_required(VERSION 3.28)
project(n19)

# Cmake Configurations
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Build options
option(ENABLE_ASAN "clang asan" ON)

set(N19_ENUMERATE_GLOBAL_SOURCES
  Frontend/ErrorCollector.cpp
  Frontend/EntityTable.cpp
  Frontend/Entity.cpp
  Frontend/DumpAst.cpp
  Frontend/Lexer.cpp
  Frontend/Token.cpp
  Frontend/FrontendContext.cpp
  Frontend/Parser.cpp
  Frontend/CompilationCycle.cpp
  Frontend/Interpreter.cpp
  Frontend/StringPool.cpp
  System/Error.cpp
  System/IODevice.cpp
  System/Time.cpp
  System/BackTrace.cpp
  System/File.cpp
  System/Process.cpp
  Core/Panic.cpp
  Core/ArgParse.cpp
  Core/StringUtil.cpp
  IO/Console.cpp
  IO/Stream.cpp
)

set(N19_ENUMERATE_GLOBAL_HEADERS
  System/String.hpp
  System/Error.hpp
  System/IODevice.hpp
  System/Handle.hpp
  System/Time.hpp
  System/BackTrace.hpp
  System/File.hpp
  System/Win32.hpp
  System/Process.hpp
  Frontend/Token.hpp
  Frontend/ErrorCollector.hpp
  Frontend/AstNodes.hpp
  Frontend/EntityTable.hpp
  Frontend/Entity.hpp
  Frontend/Lexer.hpp
  Frontend/Keywords.hpp
  Frontend/ParseContext.hpp
  Frontend/Parser.hpp
  Frontend/FrontendContext.hpp
  Frontend/CompilationCycle.hpp
  Frontend/Interpreter.hpp
  Frontend/StringPool.hpp
  Misc/Global.hpp
  Misc/Macros.hpp
  Core/Panic.hpp
  Core/Concepts.hpp
  Core/RingBase.hpp
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
  Core/StringUtil.hpp
  IO/Console.hpp
  IO/Fmt.hpp
  IO/Stream.hpp
)

set(N19_ENUMERATE_SUITES_CORE
  Tests/Suites/Core/SuiteMaybe.cpp
  Tests/Suites/Core/SuiteResult.cpp
  Tests/Suites/Core/SuiteArgParse.cpp
  Tests/Suites/Core/SuiteDefer.cpp
  Tests/Suites/Core/SuiteNothing.cpp
  Tests/Suites/Core/SuiteBytes.cpp
  Tests/Suites/Core/SuiteMurmur3.cpp
  Tests/Suites/Core/SuiteRingStructures.cpp
  Tests/Suites/Core/SuiteStringUtil.cpp
)

set(N19_ENUMERATE_SUITES_FRONTEND
  Tests/Suites/Frontend/SuiteLexer.cpp
  Tests/Suites/Frontend/SuiteEntity.cpp
)

set(N19_ENUMERATE_SUITES_IO
  Tests/Suites/IO/SuiteStream.cpp
)

set(N19_ENUMERATE_SUITES_SYSTEM
  Tests/Suites/System/SuiteSystemError.cpp
)

set(N19_ENUMERATE_PRIMARY_EXECUTABLES
  n19      # Main compiler executable
  bulwark  # The unit test executable
)

set(N19_BUILD_PLATFORM ${CMAKE_HOST_SYSTEM_NAME})
set(N19_IS_DARWIN OFF)
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
  set(N19_IS_DARWIN ON)
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
  ${N19_ENUMERATE_SUITES_CORE}
  ${N19_ENUMERATE_SUITES_FRONTEND}
  ${N19_ENUMERATE_SUITES_IO}
  ${N19_ENUMERATE_SUITES_SYSTEM}
  ${N19_ENUMERATE_GLOBAL_SOURCES}
  ${N19_ENUMERATE_GLOBAL_HEADERS}
)

function(add_platform_macros target)
  if(N19_IS_LINUX)
    target_compile_definitions(${target} PRIVATE N19_POSIX)
    target_compile_definitions(${target} PRIVATE N19_LINUX)
  elseif(N19_IS_DARWIN)
    target_compile_definitions(${target} PRIVATE N19_POSIX)
    target_compile_definitions(${target} PRIVATE N19_DARWIN)
  else()
    target_compile_definitions(${target} PRIVATE N19_WIN32)
  endif()
endfunction()

function(enable_clang_sanitizer target) 
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(REMOVE_AT ARGV 0)
    string(REPLACE ";" "," sanitizer "${ARGV}")
    message(STATUS "Build Target ${target} -- Clang Sanitizer Enabled: ${sanitizer}")
    target_compile_options(${target} PRIVATE -fsanitize=${sanitizer} -g -fno-omit-frame-pointer)
    target_link_libraries(${target} PRIVATE -fsanitize=${sanitizer})
  else()
    message(FATAL_ERROR "enable_clang_sanitizer must be used with Clang!")
  endif()
endfunction()

# Build all executables
foreach(executable ${N19_ENUMERATE_PRIMARY_EXECUTABLES})
  target_include_directories(${executable} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_platform_macros(${executable})

  # TODO: this is temporary, and can be done better.
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(NOT N19_IS_WINDOWS AND ENABLE_ASAN)
      enable_clang_sanitizer(${executable} address)
    endif()
  endif()

  if(MSVC)
    target_compile_options(${executable} PRIVATE /FI Misc/Global.hpp)
  else()
    target_compile_options(${executable} PRIVATE -include Misc/Global.hpp)
  endif()
endforeach()
