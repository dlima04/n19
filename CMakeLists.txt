cmake_minimum_required(VERSION 3.28)
project(n19)
set(CMAKE_CXX_STANDARD 23)

# The source files for the compiler.
# Header files are included here so that
# Clion can provide insight on them.
set(N19_SOURCE_FILES
  n19/Src/Main.cpp
  n19/Src/Core/ConManip.cpp
  n19/Src/Core/FileRef.cpp
  n19/Src/Frontend/Token.cpp
  n19/Src/Frontend/Lexer.cpp
  n19/Src/Frontend/ErrorCollector.cpp
  n19/Src/Frontend/EntityTable.cpp
  n19/Src/Frontend/Entity.cpp
  n19/Src/Frontend/EntityQualifier.cpp
  n19/Src/Frontend/DumpAst.cpp
  n19/Src/Core/ArgParse.cpp
  n19/Src/Native/LastError.cpp
  n19/Src/Core/RingBuffer.cpp
  n19/Src/Native/IODevice.cpp
  n19/Include/Core/ConManip.h
  n19/Include/Core/Result.h
  n19/Include/Core/ResultMacros.h
  n19/Include/Core/GlobalLastError.h
  n19/Include/Core/Panic.h
  n19/Include/Core/Fmt.h
  n19/Include/Core/FileRef.h
  n19/Include/Core/Defer.h
  n19/Include/Frontend/Lexer.h
  n19/Include/Frontend/Token.h
  n19/Include/Frontend/ErrorCollector.h
  n19/Include/Frontend/AstNodes.h
  n19/Include/Frontend/EntityQualifier.h
  n19/Include/Frontend/EntityTable.h
  n19/Include/Frontend/Entity.h
  n19/Include/Core/Concepts.h
  n19/Include/Core/StopWatch.h
  n19/Include/Core/ArgParse.h
  n19/Include/Native/String.h
  n19/Include/Native/Stream.h
  n19/Include/Native/LastError.h
  n19/Include/Core/RingBuffer.h
  n19/Include/Native/IODevice.h
  n19/Include/Native/Handle.h
  n19/Include/Core/Bytes.h
  n19/Include/Core/Attributes.h
)

add_executable(n19 ${N19_SOURCE_FILES})
target_include_directories(n19 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/n19/Include)

set(N19_BUILD_PLATFORM ${CMAKE_HOST_SYSTEM_NAME})
set(N19_IS_MACOS FALSE)
set(N19_IS_LINUX FALSE)
set(N19_IS_POSIX FALSE)
set(N19_IS_WINDOWS FALSE)

# Determine build platform...
# also, is this actually reliable?
# CMAKE_HOST_SYSTEM_NAME seems to be
# unofficial or at least volatile in some cases
# based on what I'm seeing on the internet.

if(WIN32 OR N19_BUILD_PLATFORM STREQUAL "Windows")
  set(N19_IS_WINDOWS TRUE)
elseif(N19_BUILD_PLATFORM STREQUAL "Linux")
  set(N19_IS_LINUX TRUE)
  set(N19_IS_POSIX TRUE)
elseif(N19_BUILD_PLATFORM STREQUAL "Darwin")
  set(N19_IS_MACOS TRUE)
  set(N19_IS_POSIX TRUE)
else()
  message(FATAL_ERROR "Unsupported build platform. "
    "Supported platforms: MacOS (Darwin), Linux, Windows.")
endif()

# Create definitions for the compiler
if(N19_IS_LINUX)
  target_compile_definitions(n19 PRIVATE N19_POSIX)
  target_compile_definitions(n19 PRIVATE N19_LINUX)
elseif(N19_IS_MACOS)
  target_compile_definitions(n19 PRIVATE N19_POSIX)
  target_compile_definitions(n19 PRIVATE N19_MACOS)
else()
  target_compile_definitions(n19 PRIVATE N19_WIN32)
endif()