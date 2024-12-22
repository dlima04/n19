cmake_minimum_required(VERSION 3.28)
project(n19)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(N19_ENUMERATE_CORE_INCLUDE
  Core/Panic.hpp
  Core/Fmt.hpp
  Core/FileRef.hpp
  Core/Concepts.hpp
  Core/Time.hpp
  Core/RingBase.hpp
  Core/Bytes.hpp
  Core/Platform.hpp
  Core/ClassTraits.hpp
  Core/RingBuffer.hpp
  Core/RingQueue.hpp
  Core/Murmur3.hpp
  Core/TypeTraits.hpp
  Core/Callback.hpp
  Core/Forward.hpp
  Core/Result.hpp
  Core/Maybe.hpp
  Core/ConIO.hpp
  Core/Nothing.hpp
  Core/Try.hpp
  Core/Stream.hpp
  Core/ArgParse.hpp
)

set(N19_ENUMERATE_CORE_SOURCE
  Core/ConIO.cpp
  Core/FileRef.cpp
  Core/Panic.cpp
  Core/Stream.cpp
  Core/ArgParse.cpp
  Core/Time.cpp
)

set(N19_ENUMERATE_FRONTEND_INCLUDE
  Frontend/Token.hpp
  Frontend/ErrorCollector.hpp
  Frontend/AstNodes.hpp
  Frontend/EntityQualifier.hpp
  Frontend/EntityTable.hpp
  Frontend/Entity.hpp
  Frontend/Lexer.hpp
)

set(N19_ENUMERATE_FRONTEND_SOURCE
  Frontend/Token.cpp
  Frontend/ErrorCollector.cpp
  Frontend/EntityTable.cpp
  Frontend/Entity.cpp
  Frontend/EntityQualifier.cpp
  Frontend/DumpAst.cpp
  Frontend/Lexer.cpp
)

set(N19_ENUMERATE_SYS_INCLUDE
  Sys/String.hpp
  Sys/LastError.hpp
  Sys/IODevice.hpp
  Sys/Handle.hpp
)

set(N19_ENUMERATE_SYS_SOURCE
  Sys/LastError.cpp
  Sys/IODevice.cpp
  Sys/Channel.hpp
)

# Build the main compiler executable.
add_executable(n19
  Misc/CompilerMain.cpp
  Misc/Global.hpp
  ${N19_ENUMERATE_CORE_SOURCE}
  ${N19_ENUMERATE_FRONTEND_SOURCE}
  ${N19_ENUMERATE_SYS_SOURCE}
)

target_sources(n19
  PRIVATE FILE_SET HEADERS
  FILES
    ${N19_ENUMERATE_CORE_INCLUDE}
    ${N19_ENUMERATE_FRONTEND_INCLUDE}
    ${N19_ENUMERATE_SYS_INCLUDE}
)

# Set main include directory
target_include_directories(n19 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(N19_BUILD_PLATFORM ${CMAKE_HOST_SYSTEM_NAME})
set(N19_IS_MACOS OFF)
set(N19_IS_LINUX OFF)
set(N19_IS_POSIX OFF)
set(N19_IS_WINDOWS OFF)

# Determine build platform...
# also, is this actually reliable?
# Need to look into this more...

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

# Set global includes
if(MSVC)
  target_compile_options(n19 PUBLIC /FI Misc/Global.hpp)
else()
  target_compile_options(n19 PUBLIC -include Misc/Global.hpp)
endif()
