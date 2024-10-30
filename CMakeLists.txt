cmake_minimum_required(VERSION 3.15)
project(TRLC_ENUM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TRLC_BUILD_TESTS "Enable building tests (ON or OFF)" ON)
option(TRLC_GENERATE_RECURSIVE_MACRO "Enable recursive macro generation (ON or OFF)" ON)

add_library(enum INTERFACE)
add_library(trlc::enum ALIAS enum)

target_include_directories(enum INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)

if(MSVC)
    target_compile_options(enum INTERFACE
        "/Zc:preprocessor" # Use standard-compliant preprocessor behavior
        "/permissive-" # Force standards compliance
    )
endif()

if(TRLC_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(TRLC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(TRLC_GENERATE_RECURSIVE_MACRO)
    find_package(Python3 REQUIRED)

    if(NOT Python3_FOUND)
        message(FATAL_ERROR "Python 3 not found. Please install Python 3 and ensure it's in your PATH.")
    endif()

    # Command to run the Python script
    set(PYTHON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/macro_expansion_generator.py")

    if(NOT DEFINED TRLC_MACRO_RECURSIVE_MAX_NUM OR TRLC_MACRO_RECURSIVE_MAX_NUM STREQUAL "")
        message(STATUS "TRLC_MACRO_RECURSIVE_MAX_NUM is not defined. Using default value 64.")
        set(TRLC_MACRO_RECURSIVE_MAX_NUM 64)
    endif()

    set(RECURSIVE_MACRO_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/trlc/macro)
    file(MAKE_DIRECTORY ${RECURSIVE_MACRO_OUTPUT_DIRECTORY})
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${PYTHON_SCRIPT} ${TRLC_MACRO_RECURSIVE_MAX_NUM} ${RECURSIVE_MACRO_OUTPUT_DIRECTORY}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE result
    )

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Generate recursive expansions failed: ${result}.")
    else()
        message(STATUS "Generate recursive expansions executed successfully.")
    endif()
endif()

set(TRLC_ENUM_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/")

include(CMakePackageConfigHelpers)

install(TARGETS enum
    EXPORT enumTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/trlc
    INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
install(DIRECTORY "${TRLC_ENUM_HEADER_PATH}" # source directory
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include # target directory
    FILES_MATCHING # install only matched files
    PATTERN "*.hpp" # select header files
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/trlcConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/trlcConfig.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/trlc"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/trlcConfig.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/trlc"
)

install(EXPORT enumTargets
  NAMESPACE trlc::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/trlc
)
