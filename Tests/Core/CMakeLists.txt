add_library(TestCore OBJECT
  SuiteArgParse.cpp
  SuiteBytes.cpp
  SuiteDefer.cpp
  SuiteMaybe.cpp
  SuiteMurmur3.cpp
  SuiteResult.cpp
  SuiteStream.cpp
  SuiteStringUtil.cpp
  SuiteStringPool.cpp
)

target_link_libraries(TestCore PUBLIC
  project_options
  project_warnings
  Core
  Catch2::Catch2WithMain
)
