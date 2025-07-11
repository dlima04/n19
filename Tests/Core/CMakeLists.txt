add_library(TestCore OBJECT
  SuiteArgParse.cpp
  SuiteBytes.cpp
  SuiteDefer.cpp
  SuiteMaybe.cpp
  SuiteMurmur3.cpp
  SuiteResult.cpp
  SuiteRingStructures.cpp
  SuiteStream.cpp
  SuiteStringUtil.cpp
)

target_link_libraries(TestCore PUBLIC
  project_options
  project_warnings
  Core
  Catch2::Catch2WithMain
)
