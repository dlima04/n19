include(Common)

set(FRAMEWORK_SOURCES
  Useless.cpp
)

add_library(Framework STATIC
  ${FRAMEWORK_SOURCES}
)

target_link_libraries(Framework PUBLIC
  Core
)

add_executable(n19 CLIMain.cpp)
target_link_libraries(n19 PRIVATE
  project_options
  project_warnings
  Framework
)