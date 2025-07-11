macro(n19_sanitizer_error_if_not_clang)
  if(NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    message(FATAL_ERROR "Cannot build with Clang sanitizers - wrong compiler.")
  endif()
endmacro()

macro(n19_target_link_clang_sanitizer target visibility)
  n19_sanitizer_error_if_not_clang()
  string(REPLACE ";" "," sanitizer "${ARGN}")
  target_link_libraries(${target} ${visibility} -fsanitize=${ARGN})
  message(STATUS "Build Target ${target} -- Linking Clang Sanitizer(s): ${sanitizer}")
endmacro()

macro(n19_target_configure_clang_sanitizer target visibility)
  n19_sanitizer_error_if_not_clang()
  string(REPLACE ";" "," sanitizer "${ARGN}")
  target_compile_options(${target} ${visibility}
    -fsanitize=${ARGN}
    -g
    -fno-omit-frame-pointer
  )
  message(STATUS "Build Target ${target} -- Sanitizer(s) Added to CL-opt: ${sanitizer}")
endmacro()

macro(n19_link_clang_sanitizer)
  n19_sanitizer_error_if_not_clang()
  string(REPLACE ";" "," sanitizer "${ARGN}")
  message(STATUS "Linking Clang Sanitizer(s): ${sanitizer}")
  link_libraries(-fsanitize=${ARGN})
endmacro()

macro(n19_configure_clang_sanitizer)
  n19_sanitizer_error_if_not_clang()
  string(REPLACE ";" "," sanitizer "${ARGN}")
  add_compile_options(
    -fsanitize=${ARGN}
    -g
    -fno-omit-frame-pointer
  )
  message(STATUS "Sanitizer(s) Added to CL-opt: ${sanitizer}")
endmacro()
