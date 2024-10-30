cmake_minimum_required(VERSION 3.15)
project(TRLC_ENUM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TRLC_BUILD_TESTS "Enable building tests (ON or OFF)" ON)
option(TRLC_BUILD_EXAMPLES "Enable building tests (ON or OFF)" ON)
option(TRLC_GENERATE_RECURSIVE_MACRO "Enable recursive macro generation (ON or OFF)" ON)

add_library(enum INTERFACE)
add_library(trlc::enum ALIAS enum)

target_include_directories(enum INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(enum INTERFACE cxx_std_17)

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

if(UNIX OR VXWORKS)
    include(GNUInstallDirs)

    set(TRLC_ENUM_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
    set(TRLC_ENUM_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
    set(TRLC_ENUM_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(TRLC_ENUM_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(TRLC_ENUM_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/trlc")
elseif(WIN32)
    set(TRLC_ENUM_INCLUDE_INSTALL_DIR "include")
    set(TRLC_ENUM_RUNTIME_INSTALL_DIR "bin")
    set(TRLC_ENUM_LIBRARY_INSTALL_DIR "bin")
    set(TRLC_ENUM_ARCHIVE_INSTALL_DIR "lib")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(TRLC_ENUM_CMAKE_CONFIG_INSTALL_DIR "share/cmake/trlc")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else()
    message(FATAL_ERROR "Could not set install folders for this platform!")
endif()

include(CMakePackageConfigHelpers)

install(TARGETS enum EXPORT enumTargets
    RUNTIME DESTINATION ${TRLC_ENUM_RUNTIME_INSTALL_DIR}
    LIBRARY DESTINATION ${TRLC_ENUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${TRLC_ENUM_ARCHIVE_INSTALL_DIR}
    INCLUDES DESTINATION ${TRLC_ENUM_INCLUDE_INSTALL_DIR}
)

install(DIRECTORY "${TRLC_ENUM_HEADER_PATH}"
    DESTINATION ${TRLC_ENUM_INCLUDE_INSTALL_DIR}
    FILES_MATCHING
    PATTERN "*.hpp"
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/trlcConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/trlcConfig.cmake
    INSTALL_DESTINATION ${TRLC_ENUM_CMAKE_CONFIG_INSTALL_DIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/trlcConfig.cmake
    DESTINATION ${TRLC_ENUM_CMAKE_CONFIG_INSTALL_DIR}
)

install(EXPORT enumTargets
    NAMESPACE trlc::
    DESTINATION ${TRLC_ENUM_CMAKE_CONFIG_INSTALL_DIR}
)
