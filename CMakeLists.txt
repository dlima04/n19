cmake_minimum_required(VERSION 3.28)
project(n19)

set(CMAKE_CXX_STANDARD 23)
set(N19_SOURCE_FILES
  n19/Src/Main.cpp
  n19/Src/ConManip.cpp
  n19/Include/ConManip.h
  n19/Include/Result.h
  n19/Include/ResultMacros.h
  n19/Include/GlobalLastError.h
        n19/Include/Panic.h
        n19/Include/Fmt.h
        n19/Include/FileRef.h
        n19/Src/FileRef.cpp
)

add_executable(n19 ${N19_SOURCE_FILES})
target_include_directories(n19 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/n19/Include)

if(WIN32)
  target_compile_definitions(n19 PRIVATE N19_WIN32)
else()
  target_compile_definitions(n19 PRIVATE N19_POSIX)
endif()


