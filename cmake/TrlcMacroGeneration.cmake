# TrlcMacroGeneration.cmake
# Module for generating TRLC variadic macros using Python script

#[=======================================================================[.rst:
TrlcMacroGeneration
-------------------

Provides functions to generate TRLC variadic macro headers.

Functions
^^^^^^^^^

.. command:: trlc_generate_macros

  Generate variadic macro expansion headers using Python script.

  ::

    trlc_generate_macros(
      TARGET <target>
      LIMIT <limit>
      OUTPUT_DIR <dir>
      SCRIPT <script>
      [CONFIG_TEMPLATE <template>]
      [CONFIG_OUTPUT <output>]
    )

  ``TARGET``
    Target to add generated headers to.

  ``LIMIT``
    Maximum number of variadic arguments (default: 64).

  ``OUTPUT_DIR``
    Directory where macro headers will be generated.

  ``SCRIPT``
    Path to the Python script for generation.

  ``CONFIG_TEMPLATE``
    Optional: Path to config.hpp.in template.

  ``CONFIG_OUTPUT``
    Optional: Path where config.hpp will be generated.

#]=======================================================================]

function(trlc_generate_macros)
    set(options "")
    set(oneValueArgs TARGET LIMIT OUTPUT_DIR SCRIPT CONFIG_TEMPLATE CONFIG_OUTPUT)
    set(multiValueArgs "")
    
    cmake_parse_arguments(TRLC_GEN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Validate required arguments
    if(NOT TRLC_GEN_TARGET)
        message(FATAL_ERROR "trlc_generate_macros: TARGET argument is required")
    endif()
    
    if(NOT TRLC_GEN_SCRIPT)
        message(FATAL_ERROR "trlc_generate_macros: SCRIPT argument is required")
    endif()
    
    if(NOT TRLC_GEN_OUTPUT_DIR)
        message(FATAL_ERROR "trlc_generate_macros: OUTPUT_DIR argument is required")
    endif()
    
    # Set default limit if not provided
    if(NOT TRLC_GEN_LIMIT)
        set(TRLC_GEN_LIMIT 64)
    endif()
    
    # Find Python3
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    
    if(NOT Python3_FOUND)
        message(FATAL_ERROR "Python 3 not found. Please install Python 3 to generate macros.")
    endif()
    
    # Create output directory
    file(MAKE_DIRECTORY "${TRLC_GEN_OUTPUT_DIR}")
    
    # Execute Python script to generate macros
    message(STATUS "Generating TRLC variadic macros with limit=${TRLC_GEN_LIMIT}")
    execute_process(
        COMMAND ${Python3_EXECUTABLE} "${TRLC_GEN_SCRIPT}" ${TRLC_GEN_LIMIT} "${TRLC_GEN_OUTPUT_DIR}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE GENERATION_RESULT
        OUTPUT_VARIABLE GENERATION_OUTPUT
        ERROR_VARIABLE GENERATION_ERROR
    )
    
    if(NOT GENERATION_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to generate TRLC macros:\n${GENERATION_ERROR}")
    else()
        message(STATUS "Successfully generated TRLC variadic macros")
    endif()
    
    # Generate config header if template provided
    if(TRLC_GEN_CONFIG_TEMPLATE AND TRLC_GEN_CONFIG_OUTPUT)
        get_filename_component(CONFIG_OUTPUT_DIR "${TRLC_GEN_CONFIG_OUTPUT}" DIRECTORY)
        file(MAKE_DIRECTORY "${CONFIG_OUTPUT_DIR}")
        
        # Set variable for configure_file
        set(TRLC_VARIADIC_MACRO_LIMIT ${TRLC_GEN_LIMIT})
        
        configure_file(
            "${TRLC_GEN_CONFIG_TEMPLATE}"
            "${TRLC_GEN_CONFIG_OUTPUT}"
            @ONLY
        )
        
        message(STATUS "Generated config header: ${TRLC_GEN_CONFIG_OUTPUT}")
        
        # Add binary include directory to target
        get_filename_component(CONFIG_INCLUDE_DIR "${CONFIG_OUTPUT_DIR}" DIRECTORY)
        target_include_directories(${TRLC_GEN_TARGET} INTERFACE
            $<BUILD_INTERFACE:${CONFIG_INCLUDE_DIR}>
        )
    endif()
    
endfunction()
