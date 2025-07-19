include(Common)

add_library(Core STATIC
  ArgParse.cpp
  Console.cpp
  Panic.cpp
  Stream.cpp
  StringUtil.cpp
  StringPool.cpp
)

target_link_libraries(Core PUBLIC
  project_options
  project_warnings
  System
)
