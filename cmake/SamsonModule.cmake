
macro(samson_source_group)
	samson_debug_message("ARGN: ${ARGN}")
	foreach(FILE ${ARGN}) 
		# For each file, split its parent directory obtain into its relative group and its ${CMAKE_CURRENT_SOURCE_DIR}
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)    
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
		samson_debug_message("PARENT DIR: ${PARENT_DIR}")
		samson_debug_message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
		string(REPLACE "/" "\\\\" GROUP "${GROUP}")      		
		samson_debug_message("GROUP: ${GROUP}")		
		
		# Add the file to the group
		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endmacro()

macro(samson_install_dependencies target_name)
string(REGEX REPLACE "([][+.*()^$])" "\\\\\\1" ESCAPED_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
install(CODE "
cmake_policy(SET CMP0207 NEW)
message(STATUS \"installing dependencies of $<TARGET_FILE:${target_name}>\")
file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES $<TARGET_FILE:${target_name}>
        RESOLVED_DEPENDENCIES_VAR _r_deps
        UNRESOLVED_DEPENDENCIES_VAR _u_deps
        POST_EXCLUDE_REGEXES \"^C:/Windows/.*\" \"^C:/WINDOWS/.*\"
      )
      foreach(_file \${_r_deps})
      file(INSTALL
        DESTINATION \"${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>\"
        TYPE SHARED_LIBRARY
        FOLLOW_SYMLINK_CHAIN
        FILES \${_file}\
      )
    endforeach()
    list(LENGTH _u_deps _u_length)
    if(\"${_u_length}\" GREATER 0)
      message(WARNING \"Unresolved dependencies detected! -> \${_u_deps}\")
    endif()
")
endmacro()

macro(samson_unit_test module_name)
  set (CMAKE_CXX_STANDARD 20)
  find_package(GTest CONFIG REQUIRED)
  
  #Set test target name variable
  set(TEST_TARGET_NAME ${module_name}_tests)

  project(${TEST_TARGET_NAME})
  include_directories(
  )

  # Create the test executable
  add_executable(${TEST_TARGET_NAME}
      ${module_name}_test.cpp
  )

  file(GLOB_RECURSE INPUT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/input/*.*")
  samson_debug_message("INPUT_FILES: ${INPUT_FILES}")
  samson_source_group(${INPUT_FILES})
  target_sources(${TEST_TARGET_NAME} PRIVATE ${INPUT_FILES})
  samson_debug_message("INPUT_FILES2: ${INPUT_FILES}")

  
  # Ensure input files are copied to the build directory under 'input'
  foreach(input_file ${INPUT_FILES})
    get_filename_component(file_name ${input_file} NAME)
    samson_debug_message("file_name ${file_name}")
    samson_debug_message("${CMAKE_CURRENT_BINARY_DIR}/input/${file_name}")
    file(RELATIVE_PATH RELATIVE_PATH_INPUT_VAR "${CMAKE_CURRENT_SOURCE_DIR}" "${input_file}")
    samson_debug_message("RELATIVE_PATH_INPUT_VAR: ${RELATIVE_PATH_INPUT_VAR}")
    configure_file(${input_file} ${CMAKE_CURRENT_BINARY_DIR}/${RELATIVE_PATH_INPUT_VAR} COPYONLY)
  endforeach()

  set_target_properties(${TEST_TARGET_NAME} PROPERTIES FOLDER "${RELATIVE_PATH_VAR}")
  set(test_env_paths "PATH=${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>;%PATH%")
  set_target_properties(${TEST_TARGET_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "${test_env_paths}")
  
  # Link against GTest and ${module_name} library 
  message(STATUS "Unit test module name: ")
  message(STATUS ${module_name})
  target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${module_name} GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
  

  if(NOT "${ARGN}" STREQUAL "")
    samson_debug_message("my lib: ${ARGN}")
    target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${ARGN})
  endif()

  # Install the test
  install(TARGETS ${TEST_TARGET_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>")
  install(FILES $<TARGET_PDB_FILE:${TEST_TARGET_NAME}> DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>" OPTIONAL)
  samson_install_dependencies(${TEST_TARGET_NAME})

  # Register test with CTest
  add_test(NAME ${TEST_TARGET_NAME} 
    COMMAND ${TEST_TARGET_NAME}
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>"
    )
endmacro()

macro(samson_relative_headers)
 set(RELATIVE_HEADERS "")
 foreach(FILE ${ARGN}) 
    file(RELATIVE_PATH RELATIVE_PATH_HEADER_VAR "${CMAKE_CURRENT_SOURCE_DIR}" "${FILE}")
    set(RELATIVE_HEADERS ${RELATIVE_HEADERS} ${RELATIVE_PATH_HEADER_VAR})    
  endforeach()
endmacro()

# samson_define_module(module_name)
macro(samson_define_module module_name)  

	set (CMAKE_CXX_STANDARD 20)
	# Set default visibility to hidden for all targets
	set(CMAKE_CXX_VISIBILITY_PRESET hidden)
	set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

	# Define the project
	project(${module_name})
	set(CURRENT_MODULE_NAME ${module_name})
	samson_debug_message("MODULE: ${module_name}")
	add_library(${module_name} SHARED)
	
	# Generates suitable export header (${_module_name}_export.h) and definition (${module_name_UPPER}_EXPORT)
	include(GenerateExportHeader)
	string(TOLOWER ${module_name} _module_name)
	generate_export_header(${module_name})

	# Set relative path for IDE
	file(RELATIVE_PATH RELATIVE_PATH_VAR "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	string(REGEX REPLACE "/${module_name}" "" RELATIVE_PATH_VAR "${RELATIVE_PATH_VAR}") 	# Do not use module_name
	set_target_properties(${module_name} PROPERTIES FOLDER "${RELATIVE_PATH_VAR}")
	
	# Collect files required for building the module
	file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
	file(GLOB_RECURSE PRIVATE_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
	file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

	# Add target source files for building the module
	target_sources(${module_name} PRIVATE ${SOURCE_FILES})
	target_sources(${module_name} PRIVATE ${PRIVATE_HEADER_FILES})
	target_sources(${module_name} PUBLIC FILE_SET headers TYPE HEADERS FILES ${HEADER_FILES})
	target_sources(${module_name} PUBLIC FILE_SET api TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR} FILES "${CMAKE_CURRENT_BINARY_DIR}/${_module_name}_export.h")
	target_include_directories(${module_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

	# Create source groups for IDE generation
	samson_source_group(${SOURCE_FILES})  
	samson_source_group(${PRIVATE_HEADER_FILES})  
	samson_source_group(${HEADER_FILES})  
	source_group("generated" FILES "${PROJECT_BINARY_DIR}/${module_name}_export.h" )

	# Install the module
	install(TARGETS ${module_name} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>")  # fix this stuff
	install(FILES $<TARGET_PDB_FILE:${module_name}> DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>" OPTIONAL)

endmacro()