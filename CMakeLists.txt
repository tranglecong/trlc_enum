cmake_minimum_required(VERSION 3.15)
project(TRLC_ENUM)

if(MSVC)
    add_compile_options(
        "/Zc:preprocessor" # Use standard-compliant preprocessor behavior
        "/permissive-" # Force standards compliance
    )
endif()

add_library(trlc_enum INTERFACE)
add_library(trlc::enum ALIAS trlc_enum)

target_include_directories(trlc_enum INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)

set(TRLC_BUILD_TESTS ON)

if(TRLC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(TRLC_REGENERATE_RECURSIVE_MACRO)
    find_package(Python3 REQUIRED)

    if(NOT Python3_FOUND)
        message(FATAL_ERROR "Python 3 not found. Please install Python 3 and ensure it's in your PATH.")
    endif()

    # Command to run the Python script
    set(PYTHON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/macro_expansion_generator.py")

    if(NOT DEFINED TRLC_REGENERATE_RECURSIVE_NUM OR TRLC_REGENERATE_RECURSIVE_NUM STREQUAL "")
        message(WARNING "TRLC_REGENERATE_RECURSIVE_NUM is not defined. Using default value 64.")
        set(TRLC_REGENERATE_RECURSIVE_NUM 64)
    endif()

    set(RECURSIVE_MACRO_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/common/macro)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${PYTHON_SCRIPT} ${TRLC_REGENERATE_RECURSIVE_NUM} ${RECURSIVE_MACRO_OUTPUT_DIRECTORY}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE result
    )

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Generate recursive expansions failed: ${result}.")
    else()
        message(STATUS "Generate recursive expansions executed successfully.")
    endif()
endif()

if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(TRLC_ENUM_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/")
    install(TARGETS common
        EXPORT trlcTargets # Export this target
        DESTINATION ${TRLC_LIBRARY_INSTALL_DIR} # Install location for libraries
        INCLUDES DESTINATION ${TRLC_INC_INSTALL_DIR} # Install location for headers
    )
    install(DIRECTORY "${TRLC_ENUM_HEADER_PATH}" # source directory
        DESTINATION ${TRLC_INC_INSTALL_DIR} # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.hpp" # select header files
    )
endif()
