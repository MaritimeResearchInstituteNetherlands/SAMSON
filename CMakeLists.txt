cmake_minimum_required(VERSION 3.27)
set (CMAKE_CXX_STANDARD 20)

if(POLICY CMP0167)
  # vcpkg does not supply a boost-config; by setting CMP0167,
  # we suppress the warnings
  cmake_policy(SET CMP0167 OLD)
endif()

string(REGEX REPLACE "\\\\" "/" VCPKG_REPOSITORY $ENV{VCPKG_ROOT})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/vcpkg-configuration.json.in" "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg-configuration.json" @ONLY)
project(SamsonCore LANGUAGES CXX)
enable_testing()

#SamsonCore_BINARY_DIR
set(CMAKE_INSTALL_PREFIX "${SamsonCore_BINARY_DIR}/install/dev/")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin/$<CONFIG>")

message(STATUS "PROJECT NAME: ${PROJECT_NAME}")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SamsonUtils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SamsonModule.cmake)
#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SamsonDoxygen.cmake)
#samson_add_doxygen()

#Set default visibility to hidden for all targets
#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

#External packages
set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost 1.83.0 REQUIRED
 COMPONENTS serialization 
)
find_package(shapelib CONFIG REQUIRED)
find_package(yaml-cpp REQUIRED)

#SAMPLE CLI(Command Line Interface) EXE
add_executable(SamsonCLI CLI/main.cpp)
target_link_libraries(SamsonCLI sip)

#Add CLI sample files to the project
file(GLOB_RECURSE CLI_SAMPLE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CLI/Sample/*.*")
samson_source_group(${CLI_SAMPLE_FILES})
target_sources(SamsonCLI PRIVATE ${CLI_SAMPLE_FILES})
foreach(sample_file ${CLI_SAMPLE_FILES})
    get_filename_component(file_name ${sample_file} NAME)    
    file(RELATIVE_PATH RELATIVE_SAMPLE_PATH_VAR "${CMAKE_CURRENT_SOURCE_DIR}/../" "${sample_file}")
    configure_file(${sample_file} ${CMAKE_CURRENT_BINARY_DIR}/${RELATIVE_SAMPLE_PATH_VAR} COPYONLY)
endforeach()

    
#SAMSONCORE DLL
add_library(${PROJECT_NAME}
    SHARED src/SamsonCore.cpp    
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION 0.0.1
    SOVERSION 0
)

target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET api
        TYPE HEADERS
        BASE_DIRS include
        FILES include/SamsonCore/SamsonCore.h
)

#LINK EXTERNALS
add_subdirectory("external/TrafficDatabase")
add_subdirectory("external/AccidentModel")
add_subdirectory("external/Utilities")

#target_link_libraries(${PROJECT_NAME} PUBLIC)

#Generates suitable export header (samsoncore_export.h) and definition (SAMSONCORE_EXPORT), 
# adds export definition (SamsonCore_EXPORTS) to lib during the build
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET api
        TYPE HEADERS
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/samsoncore_export.h
)


#INSTALLING
#install(TARGETS SamsonCLI FILE_SET api) 
install(TARGETS SamsonCLI DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>") # This can be done better....
samson_install_dependencies("SamsonCLI")
message(STATUS "Target folder for pdb: ${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>/")
install(FILES $<TARGET_PDB_FILE:SamsonCLI> DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>" OPTIONAL)
#install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/\$<CONFIG>" OPTIONAL)

#Install CLI sample files
foreach(sample_file ${CLI_SAMPLE_FILES})
    file(RELATIVE_PATH RELATIVE_SAMPLE_PATH_VAR "${CMAKE_CURRENT_SOURCE_DIR}/CLI" "${sample_file}")
    get_filename_component(folder_name ${RELATIVE_SAMPLE_PATH_VAR} PATH)
    install(FILES ${sample_file} DESTINATION ${folder_name})
endforeach()

#PACKAGING
# These are project-specific
set(CPACK_PACKAGE_NAME SamsonCLI )
set(CPACK_PACKAGE_VENDOR MARIN)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SAMSON Command Line Interface")
# These lines tend to be the same for every project
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX})
set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_VERBATIM_VARIABLES TRUE)

if(WIN32)
    #set(CPACK_GENERATOR ZIP WIX)
    set(CPACK_GENERATOR ZIP)
elseif(APPLE)
    set(CPACK_GENERATOR TGZ productbuild)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR TGZ RPM)
else()
    set(CPACK_GENERATOR TGZ)
endif()

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "4")

# This is what writes out the input file for cpack
include(CPack)

#TESTING
add_subdirectory(tests)
add_subdirectory(CLI/test)
