# TrlcCompilerWarnings.cmake
# Centralized compiler warning configuration for TRLC library

#[=======================================================================[.rst:
TrlcCompilerWarnings
--------------------

Provides functions to configure compiler warnings.

Functions
^^^^^^^^^

.. command:: trlc_set_warnings

  Set compiler warnings for a target.

  ::

    trlc_set_warnings(
      TARGET <target>
      [WARNINGS_AS_ERRORS]
    )

  ``TARGET``
    The target to apply warnings to.

  ``WARNINGS_AS_ERRORS``
    Treat warnings as errors (strict mode).

#]=======================================================================]

function(trlc_set_warnings)
    set(options WARNINGS_AS_ERRORS)
    set(oneValueArgs TARGET)
    set(multiValueArgs "")
    
    cmake_parse_arguments(TRLC_WARN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT TRLC_WARN_TARGET)
        message(FATAL_ERROR "trlc_set_warnings: TARGET argument is required")
    endif()
    
    # Get target type
    get_target_property(TARGET_TYPE ${TRLC_WARN_TARGET} TYPE)
    
    # Determine if target is interface
    if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
        set(VISIBILITY INTERFACE)
    else()
        set(VISIBILITY PUBLIC)
    endif()
    
    # GCC and Clang warnings
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TRLC_WARN_TARGET} ${VISIBILITY}
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wsign-conversion
            -Wformat=2
            -Wimplicit-fallthrough
        )
        
        if(TRLC_WARN_WARNINGS_AS_ERRORS)
            target_compile_options(${TRLC_WARN_TARGET} ${VISIBILITY} -Werror)
        endif()
    endif()
    
    # MSVC warnings
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${TRLC_WARN_TARGET} ${VISIBILITY}
            /W4           # Warning level 4
            /permissive-  # Standards conformance
            /Zc:preprocessor  # Standard preprocessor
            /Zc:__cplusplus   # Correct __cplusplus macro
        )
        
        if(TRLC_WARN_WARNINGS_AS_ERRORS)
            target_compile_options(${TRLC_WARN_TARGET} ${VISIBILITY} /WX)
        endif()
        
        # Disable specific MSVC warnings that conflict with standard code
        target_compile_options(${TRLC_WARN_TARGET} ${VISIBILITY}
            /wd4251  # 'identifier' : class 'type' needs to have dll-interface
            /wd4275  # non dll-interface class used as base for dll-interface class
        )
    endif()
    
endfunction()
