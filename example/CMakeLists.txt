# Define the list of example
set(TEST_SOURCES
    example.cpp
)

# Loop through each test source and create the corresponding executable
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE trlc::enum)
endforeach()