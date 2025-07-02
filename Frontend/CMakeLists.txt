include(Common)

add_executable(rl
  AST/DumpAST.cpp
  Common/CompilationCycle.cpp
  Diagnostics/ErrorCollector.cpp
  Entities/Entity.cpp
  Entities/EntityTable.cpp
  Lexer/Lexer.cpp
  Lexer/Token.cpp
  Parser/Parser.cpp
)

target_link_libraries(rl PRIVATE
  project_options
  project_warnings
  Core
  System
)