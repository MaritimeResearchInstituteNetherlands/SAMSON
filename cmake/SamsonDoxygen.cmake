# Doxygen.cmake
macro(samson_add_doxygen)
    # Add an option to enable or disable Doxygen documentation generation
    option(SAMSON_ENABLE_DOXYGEN "Enable generation of Doxygen documentation" ON)

    # Check if the option is enabled
    if(SAMSON_ENABLE_DOXYGEN)
        # Find the Doxygen package
        find_package(Doxygen REQUIRED)

        # Set input and output directories
        set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)

        # Configure Doxygen
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${DOXYGEN_OUTPUT_DIR}/html)

        # Check if the Doxygen configuration file exists
        if(NOT EXISTS ${DOXYGEN_IN})
            message(FATAL_ERROR "Doxyfile not found. Please create a Doxyfile using 'doxygen -g'.")
        endif()

        # Add a custom target for generating the Doxygen documentation
        add_custom_target(SamsonDoxygenCreate
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating Doxygen documentation"
            VERBATIM
        )

        # Add a custom target for cleaning the documentation
        add_custom_target(SamsonDoxygenClean
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${DOXYGEN_OUTPUT_DIR}
            COMMENT "Cleaning Doxygen documentation"
            VERBATIM
        )
    else()
        message(STATUS "Doxygen documentation generation is disabled.")
    endif()
endmacro()
