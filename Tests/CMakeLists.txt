include(Common)

# testrunner-core
add_executable(testrunner-core
  Runners/RunnerMain.cpp
  Suites/Core/SuiteArgParse.cpp
  Suites/Core/SuiteBytes.cpp
  Suites/Core/SuiteDefer.cpp
  Suites/Core/SuiteMaybe.cpp
  Suites/Core/SuiteMurmur3.cpp
  Suites/Core/SuiteResult.cpp
  Suites/Core/SuiteRingStructures.cpp
  Suites/Core/SuiteStream.cpp
  Suites/Core/SuiteStringUtil.cpp
)

target_link_libraries(testrunner-core PRIVATE
  project_options
  project_warnings
  Bulwark
)

# testrunner-system
add_executable(testrunner-system
  Runners/RunnerMain.cpp
  Suites/System/SuiteSystemError.cpp
)

target_link_libraries(testrunner-system PRIVATE
  project_options
  project_warnings
  Bulwark
)

# testrunner-rl (reference language)
add_executable(testrunner-rl
  Runners/RunnerMain.cpp
  Suites/Frontend/SuiteEntity.cpp
  Suites/Frontend/SuiteLexer.cpp
)

target_link_libraries(testrunner-rl PRIVATE
  project_options
  project_warnings
  Bulwark
  Frontend
)

# build parent test runner
add_executable(runtests Runners/ParentMain.cpp)
target_link_libraries(runtests PRIVATE
  project_options
  project_warnings
  Bulwark
)
