cmake_minimum_required(VERSION 3.28)
project(n19)

set(CMAKE_CXX_STANDARD 23)
set(N19_SOURCE_FILES
  n19/Src/Main.cpp
  n19/Src/ConManip.cpp
  n19/Src/FileRef.cpp
  n19/Src/Token.cpp
  n19/Src/Lexer.cpp
  n19/Src/ErrorCollector.cpp
  n19/Src/EntityTable.cpp
  n19/Src/Entity.cpp
  n19/Src/EntityQualifier.cpp
  n19/Include/ConManip.h
  n19/Include/Result.h
  n19/Include/ResultMacros.h
  n19/Include/GlobalLastError.h
  n19/Include/Panic.h
  n19/Include/Fmt.h
  n19/Include/FileRef.h
  n19/Include/Defer.h
  n19/Include/Lexer.h
  n19/Include/Token.h
  n19/Include/ErrorCollector.h
  n19/Include/AstNodes.h
  n19/Include/EntityQualifier.h
  n19/Include/EntityTable.h
  n19/Include/Entity.h
  n19/Src/DumpAst.cpp
        n19/Include/Concepts.h
)

add_executable(n19 ${N19_SOURCE_FILES})
target_include_directories(n19 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/n19/Include)

if(WIN32)
  target_compile_definitions(n19 PRIVATE N19_WIN32)
else()
  target_compile_definitions(n19 PRIVATE N19_POSIX)
endif()


