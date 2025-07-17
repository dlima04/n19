add_library(System STATIC
  BackTrace.cpp
  Error.cpp
  File.cpp
  IODevice.cpp
  Process.cpp
  SharedRegion.cpp
  Time.cpp
)

target_link_libraries(System PUBLIC
  project_options
  project_warnings
  Core
)
