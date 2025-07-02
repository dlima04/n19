include(Common)

add_library(Bulwark STATIC
  Case.cpp
  Registry.cpp
  Reporting.cpp
  Suite.cpp
)

target_link_libraries(Bulwark PUBLIC
  project_options
  project_warnings
  Core
  System
)
