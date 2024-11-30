cmake_minimum_required(VERSION 3.28)
project(n19)
set(CMAKE_CXX_STANDARD 23)

# The source files for the compiler.
# Header files are included here so that
# Clion can provide insight on them.
set(N19_SOURCE_FILES
  Core/ConManip.cpp
  Core/FileRef.cpp
  Frontend/Token.cpp
  Frontend/ShitLexer.cpp
  Frontend/ErrorCollector.cpp
  Frontend/EntityTable.cpp
  Frontend/Entity.cpp
  Frontend/EntityQualifier.cpp
  Frontend/DumpAst.cpp
  Frontend/Main.cpp
  Core/ArgParse.cpp
  Native/LastError.cpp
  Native/IODevice.cpp
  Core/ConManip.hpp
  Core/Result.hpp
  Core/ResultMacros.hpp
  Core/GlobalLastError.hpp
  Core/Panic.hpp
  Core/Fmt.hpp
  Core/FileRef.hpp
  Core/Defer.hpp
  Frontend/ShitLexer.hpp
  Frontend/Token.hpp
  Frontend/ErrorCollector.hpp
  Frontend/AstNodes.hpp
  Frontend/EntityQualifier.hpp
  Frontend/EntityTable.hpp
  Frontend/Entity.hpp
  Core/Concepts.hpp
  Core/StopWatch.hpp
  Core/ArgParse.hpp
  Native/String.hpp
  Native/Stream.hpp
  Native/LastError.hpp
  Core/RingBase.hpp
  Native/IODevice.hpp
  Native/Handle.hpp
  Core/Bytes.hpp
  Core/Platform.hpp
  Core/ClassTraits.hpp
  Core/RingBuffer.hpp
  Core/RingQueue.hpp
        Frontend/LexerBase.hpp
        Frontend/LazyLexer.hpp
        Frontend/BufferedLexer.hpp
        Frontend/LexerBase.cpp
        Core/Panic.cpp
        Frontend/LazyLexer.cpp
)

add_executable(n19 ${N19_SOURCE_FILES})
target_include_directories(n19 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

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