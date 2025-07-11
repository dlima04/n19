include(Common)

enable_testing()
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(catch2)

add_subdirectory(Core)
add_subdirectory(Frontend)
add_subdirectory(System)

add_executable(runtests Entry.cpp)
target_link_libraries(runtests PRIVATE TestCore)
target_link_libraries(runtests PRIVATE TestFrontend)
target_link_libraries(runtests PRIVATE TestSystem)

include(CTest)
include(Catch)
catch_discover_tests(runtests)
