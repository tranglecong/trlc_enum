# TRLC Enum Library
# Modern C++17 header-only enum library with enhanced features

cmake_minimum_required(VERSION 3.15...3.28)

# Project declaration with semantic versioning
project(TRLC_ENUM
    VERSION 1.0.0
    DESCRIPTION "A powerful C++ enum library with attributes and compile-time operations"
    HOMEPAGE_URL "https://github.com/tranglecong/trlc_enum"
    LANGUAGES CXX
)

# Add CMake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#------------------------------------------------------------------------------
# Build Options
#------------------------------------------------------------------------------

option(TRLC_BUILD_TESTS "Build unit tests" ON)
option(TRLC_BUILD_EXAMPLES "Build example applications" ON)
option(TRLC_GENERATE_MACROS "Generate variadic macros at configure time" ON)
option(TRLC_INSTALL "Generate install target" ON)
option(TRLC_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(TRLC_ENABLE_CLANG_TIDY "Enable clang-tidy checks during build" OFF)

# Cache variable for macro generation limit
set(TRLC_VARIADIC_MACRO_LIMIT "64" CACHE STRING 
    "Maximum number of variadic arguments supported by TRLC macros")

#------------------------------------------------------------------------------
# Prevent in-source builds
#------------------------------------------------------------------------------

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

#------------------------------------------------------------------------------
# Project-wide settings
#------------------------------------------------------------------------------

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Output directories for multi-config generators
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

# Enable folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#------------------------------------------------------------------------------
# Main Library Target
#------------------------------------------------------------------------------

add_library(trlc_enum INTERFACE)
add_library(trlc::enum ALIAS trlc_enum)

# Target properties
target_include_directories(trlc_enum INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(trlc_enum INTERFACE cxx_std_17)

# Set project metadata as target properties
set_target_properties(trlc_enum PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    INTERFACE_TRLC_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}
)

# MSVC-specific configuration
if(MSVC)
    target_compile_options(trlc_enum INTERFACE
        /Zc:preprocessor  # Standard-compliant preprocessor
        /permissive-      # Standards conformance mode
        /Zc:__cplusplus   # Enable updated __cplusplus macro
    )
endif()

#------------------------------------------------------------------------------
# Compiler Warnings
#------------------------------------------------------------------------------

include(TrlcCompilerWarnings)

if(TRLC_WARNINGS_AS_ERRORS)
    trlc_set_warnings(TARGET trlc_enum WARNINGS_AS_ERRORS)
else()
    trlc_set_warnings(TARGET trlc_enum)
endif()

#------------------------------------------------------------------------------
# Clang-Tidy Integration
#------------------------------------------------------------------------------

if(TRLC_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found")
    endif()
endif()

#------------------------------------------------------------------------------
# Variadic Macro Generation
#------------------------------------------------------------------------------

if(TRLC_GENERATE_MACROS)
    include(TrlcMacroGeneration)
    
    # Validate macro limit
    if(NOT TRLC_VARIADIC_MACRO_LIMIT MATCHES "^[0-9]+$")
        message(FATAL_ERROR "TRLC_VARIADIC_MACRO_LIMIT must be a positive integer, got: ${TRLC_VARIADIC_MACRO_LIMIT}")
    endif()
    
    if(TRLC_VARIADIC_MACRO_LIMIT LESS 1 OR TRLC_VARIADIC_MACRO_LIMIT GREATER 256)
        message(FATAL_ERROR "TRLC_VARIADIC_MACRO_LIMIT must be between 1 and 256, got: ${TRLC_VARIADIC_MACRO_LIMIT}")
    endif()
    
    message(STATUS "TRLC macro generation enabled (limit: ${TRLC_VARIADIC_MACRO_LIMIT})")
    
    trlc_generate_macros(
        TARGET trlc_enum
        LIMIT ${TRLC_VARIADIC_MACRO_LIMIT}
        OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/trlc/macro"
        SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_variadic_macros.py"
        CONFIG_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in"
        CONFIG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/trlc/config.hpp"
    )
endif()

#------------------------------------------------------------------------------
# Subdirectories
#------------------------------------------------------------------------------

if(TRLC_BUILD_EXAMPLES)
    message(STATUS "Building examples")
    add_subdirectory(examples)
endif()

if(TRLC_BUILD_TESTS)
    message(STATUS "Building tests")
    enable_testing()
    add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------
# Installation
#------------------------------------------------------------------------------

if(TRLC_INSTALL)
    include(TrlcInstallation)
    
    trlc_setup_installation(
        TARGET trlc_enum
        NAMESPACE trlc::
        EXPORT_NAME TrlcTargets
        PACKAGE_NAME Trlc
        HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        CONFIG_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TrlcConfig.cmake.in"
        VERSION_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TrlcConfigVersion.cmake.in"
    )
    
    # Install generated config header if it exists
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/trlc/config.hpp")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/trlc/config.hpp"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/trlc"
        )
    endif()
endif()

#------------------------------------------------------------------------------
# Export for build tree usage
#------------------------------------------------------------------------------

# Allow using this library from the build tree without installation
export(TARGETS trlc_enum
    NAMESPACE trlc::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/TrlcTargets.cmake"
)

# Register package in CMake user package registry
export(PACKAGE Trlc)

#------------------------------------------------------------------------------
# Summary
#------------------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "")
    message(STATUS "===================================")
    message(STATUS "TRLC Enum Library Configuration")
    message(STATUS "===================================")
    message(STATUS "Version:              ${PROJECT_VERSION}")
    message(STATUS "Build type:           ${CMAKE_BUILD_TYPE}")
    message(STATUS "C++ Standard:         C++17")
    message(STATUS "")
    message(STATUS "Options:")
    message(STATUS "  Build tests:        ${TRLC_BUILD_TESTS}")
    message(STATUS "  Build examples:     ${TRLC_BUILD_EXAMPLES}")
    message(STATUS "  Generate macros:    ${TRLC_GENERATE_MACROS}")
    if(TRLC_GENERATE_MACROS)
        message(STATUS "  Macro limit:        ${TRLC_VARIADIC_MACRO_LIMIT}")
    endif()
    message(STATUS "  Install:            ${TRLC_INSTALL}")
    message(STATUS "  Warnings as errors: ${TRLC_WARNINGS_AS_ERRORS}")
    message(STATUS "  clang-tidy:         ${TRLC_ENABLE_CLANG_TIDY}")
    message(STATUS "===================================")
    message(STATUS "")
endif()
