option(SAMSON_CMAKE_DEBUG_MESSAGES "Enable debug messages for Samson CMake configuration" OFF)


macro(samson_debug_message)
  if(SAMSON_CMAKE_DEBUG_MESSAGES)
    string(REPLACE ";" " " __msg "${ARGN}")
    message(STATUS "${__msg}")
  endif()
endmacro()
