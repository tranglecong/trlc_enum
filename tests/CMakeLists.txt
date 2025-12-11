# Tests CMakeLists.txt
# Unit tests configuration using GoogleTest

#------------------------------------------------------------------------------
# Fetch GoogleTest
#------------------------------------------------------------------------------

include(FetchContent)

# Fetch GoogleTest if not found on system
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# For Windows: Prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Make GoogleTest available
FetchContent_MakeAvailable(googletest)

#------------------------------------------------------------------------------
# Test Configuration
#------------------------------------------------------------------------------

# List of test source files
set(TEST_SOURCES
    # Macro tests - low-level macro utilities
    unit/macro/test_count.cpp
    unit/macro/test_foreach_stmt.cpp
    unit/macro/test_foreach_stmt_indexed.cpp
    unit/macro/test_foreach_expr.cpp
    unit/macro/test_foreach_expr_indexed.cpp
    unit/macro/test_concat.cpp
    
    # Enum tests - public API functionality
    unit/enum/test_basic_enum.cpp
    unit/enum/test_enum_fields.cpp
    unit/enum/test_enum_iteration.cpp
    unit/enum/test_enum_lookup.cpp
    unit/enum/test_enum_policies.cpp
    
    # Integration tests - real-world scenarios
    integration/test_http_status.cpp
    integration/test_error_codes.cpp
    integration/test_state_machine.cpp
)

# Create test executable for each source
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # Create test executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Link libraries
    target_link_libraries(${TEST_NAME} 
        PRIVATE 
            trlc::enum 
            GTest::gtest 
            GTest::gtest_main
    )
    
    # Set target properties
    set_target_properties(${TEST_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        FOLDER "Tests"
    )
    
    # Register with CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        TIMEOUT 30
    )
    
    # Add labels for test filtering
    if(TEST_SOURCE MATCHES "unit/macro/")
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "macro;unit")
    elseif(TEST_SOURCE MATCHES "unit/enum/")
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "enum;unit")
    elseif(TEST_SOURCE MATCHES "integration/")
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "integration")
    else()
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "unit")
    endif()
endforeach()

#------------------------------------------------------------------------------
# Test Targets
#------------------------------------------------------------------------------

# Custom target to build all tests
add_custom_target(build_tests
    DEPENDS ${TEST_SOURCES}
    COMMENT "Building all tests"
)

# Add tests folder in IDE
if(TARGET gtest)
    set_target_properties(gtest gtest_main gmock gmock_main
        PROPERTIES FOLDER "External/GoogleTest"
    )
endif()