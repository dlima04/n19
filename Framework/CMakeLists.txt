include(Common)

# No library here yet...

#add_library(Framework STATIC
#
#)

add_executable(n19 CLIMain.cpp)
target_link_libraries(n19 PRIVATE
  project_options
  project_warnings
  Core
)