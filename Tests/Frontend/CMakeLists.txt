add_library(TestFrontend OBJECT
  SuiteLexer.cpp
  SuiteEntity.cpp
)

target_link_libraries(TestFrontend PUBLIC
  project_warnings
  project_options
  Frontend
  Catch2::Catch2WithMain
)
