# TrlcInstallation.cmake
# Module for handling TRLC library installation and export

#[=======================================================================[.rst:
TrlcInstallation
----------------

Provides functions to configure installation and package export.

Functions
^^^^^^^^^

.. command:: trlc_setup_installation

  Configure installation rules and generate package config files.

  ::

    trlc_setup_installation(
      TARGET <target>
      NAMESPACE <namespace>
      EXPORT_NAME <export_name>
      PACKAGE_NAME <package_name>
      HEADER_PATH <path>
      CONFIG_TEMPLATE <template>
      VERSION_TEMPLATE <version_template>
    )

  ``TARGET``
    The target to install.

  ``NAMESPACE``
    Namespace for exported targets (e.g., "trlc::").

  ``EXPORT_NAME``
    Name for the export set (e.g., "TrlcTargets").

  ``PACKAGE_NAME``
    Name of the package (e.g., "Trlc").

  ``HEADER_PATH``
    Path to headers directory to install.

  ``CONFIG_TEMPLATE``
    Path to package config template file.

  ``VERSION_TEMPLATE``
    Path to version config template file.

#]=======================================================================]

function(trlc_setup_installation)
    set(options "")
    set(oneValueArgs TARGET NAMESPACE EXPORT_NAME PACKAGE_NAME HEADER_PATH CONFIG_TEMPLATE VERSION_TEMPLATE)
    set(multiValueArgs "")
    
    cmake_parse_arguments(TRLC_INST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Validate required arguments
    if(NOT TRLC_INST_TARGET)
        message(FATAL_ERROR "trlc_setup_installation: TARGET is required")
    endif()
    
    if(NOT TRLC_INST_EXPORT_NAME)
        message(FATAL_ERROR "trlc_setup_installation: EXPORT_NAME is required")
    endif()
    
    if(NOT TRLC_INST_PACKAGE_NAME)
        message(FATAL_ERROR "trlc_setup_installation: PACKAGE_NAME is required")
    endif()
    
    # Set up install directories
    include(GNUInstallDirs)
    
    if(WIN32)
        set(TRLC_INCLUDE_INSTALL_DIR "include")
        set(TRLC_RUNTIME_INSTALL_DIR "bin")
        set(TRLC_LIBRARY_INSTALL_DIR "bin")
        set(TRLC_ARCHIVE_INSTALL_DIR "lib")
        set(TRLC_CMAKE_CONFIG_INSTALL_DIR "share/cmake/${TRLC_INST_PACKAGE_NAME}")
    else()
        set(TRLC_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
        set(TRLC_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
        set(TRLC_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
        set(TRLC_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
        set(TRLC_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${TRLC_INST_PACKAGE_NAME}")
    endif()
    
    # Install target
    install(TARGETS ${TRLC_INST_TARGET}
        EXPORT ${TRLC_INST_EXPORT_NAME}
        RUNTIME DESTINATION "${TRLC_RUNTIME_INSTALL_DIR}"
        LIBRARY DESTINATION "${TRLC_LIBRARY_INSTALL_DIR}"
        ARCHIVE DESTINATION "${TRLC_ARCHIVE_INSTALL_DIR}"
        INCLUDES DESTINATION "${TRLC_INCLUDE_INSTALL_DIR}"
    )
    
    # Install headers
    if(TRLC_INST_HEADER_PATH)
        install(DIRECTORY "${TRLC_INST_HEADER_PATH}"
            DESTINATION "${TRLC_INCLUDE_INSTALL_DIR}"
            FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.h"
            PATTERN "*.inl"
        )
    endif()
    
    # Install export targets
    if(TRLC_INST_NAMESPACE)
        set(NAMESPACE_ARG NAMESPACE ${TRLC_INST_NAMESPACE})
    endif()
    
    install(EXPORT ${TRLC_INST_EXPORT_NAME}
        FILE "${TRLC_INST_EXPORT_NAME}.cmake"
        ${NAMESPACE_ARG}
        DESTINATION "${TRLC_CMAKE_CONFIG_INSTALL_DIR}"
    )
    
    # Generate and install package config file
    if(TRLC_INST_CONFIG_TEMPLATE)
        include(CMakePackageConfigHelpers)
        
        configure_package_config_file(
            "${TRLC_INST_CONFIG_TEMPLATE}"
            "${CMAKE_CURRENT_BINARY_DIR}/${TRLC_INST_PACKAGE_NAME}Config.cmake"
            INSTALL_DESTINATION "${TRLC_CMAKE_CONFIG_INSTALL_DIR}"
        )
        
        install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/${TRLC_INST_PACKAGE_NAME}Config.cmake"
            DESTINATION "${TRLC_CMAKE_CONFIG_INSTALL_DIR}"
        )
    endif()
    
    # Generate and install version config file
    if(TRLC_INST_VERSION_TEMPLATE)
        include(CMakePackageConfigHelpers)
        
        write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/${TRLC_INST_PACKAGE_NAME}ConfigVersion.cmake"
            VERSION "${PROJECT_VERSION}"
            COMPATIBILITY SameMajorVersion
        )
        
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${TRLC_INST_PACKAGE_NAME}ConfigVersion.cmake"
            DESTINATION "${TRLC_CMAKE_CONFIG_INSTALL_DIR}"
        )
    endif()
    
    # Export information for parent scope
    set(TRLC_CMAKE_CONFIG_INSTALL_DIR ${TRLC_CMAKE_CONFIG_INSTALL_DIR} PARENT_SCOPE)
    
endfunction()
