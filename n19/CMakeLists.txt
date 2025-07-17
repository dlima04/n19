add_subdirectory(Core)
add_subdirectory(System)

if(BUILD_RL)
  message(STATUS "BuildOpt: build RL.")
  add_subdirectory(Frontend)
else()
  message(STATUS "BuildOpt: do not build RL.")
endif()
