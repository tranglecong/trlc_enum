# Examples CMakeLists.txt
# Build example applications demonstrating TRLC library features

# List of example source files
set(EXAMPLE_SOURCES
    basic_usage.cpp
    custom_extra.cpp
    custom_policies.cpp
)

# Create executable for each example
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    
    # Link to TRLC library
    target_link_libraries(${EXAMPLE_NAME} PRIVATE trlc::enum)
    
    # Set target properties
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        FOLDER "Examples"
    )
    
    # Add to examples folder in IDE
    if(CMAKE_FOLDER)
        set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER "Examples")
    endif()
endforeach()

# Add custom target to build all examples
add_custom_target(examples
    DEPENDS ${EXAMPLE_SOURCES}
    COMMENT "Building all examples"
)