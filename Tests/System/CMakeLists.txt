add_library(TestSystem OBJECT
  SuiteSystemError.cpp
)

target_link_libraries(TestSystem PUBLIC
  project_options
  project_warnings
  Core
  Catch2::Catch2WithMain
)
