include(Common)

set(FRONTEND_SOURCES
  AST/DumpAST.cpp
  Common/CompilationCycle.cpp
  Diagnostics/ErrorCollector.cpp
  Entities/Entity.cpp
  Entities/EntityTable.cpp
  Lexer/Lexer.cpp
  Lexer/Token.cpp
  Parser/Parser.cpp
  FrontendContext.cpp
)

add_library(Frontend STATIC
  ${FRONTEND_SOURCES}
)

# TODO: link Framework when that is created
target_link_libraries(Frontend PUBLIC
  project_options
  project_warnings
  Core
)

add_executable(rl FrontendMain.cpp)
target_link_libraries(rl PRIVATE
  project_options
  project_warnings
  Frontend
)
