find_package(GTest CONFIG REQUIRED)

#Set test target name variable
set(TEST_TARGET_NAME ${PROJECT_NAME}_Tests)

# Create the test executable
add_executable(${TEST_TARGET_NAME}
    samson_core_test.cpp
)

#Keep test next to the lib
set_target_properties(${TEST_TARGET_NAME} PROPERTIES FOLDER "${RELATIVE_PATH_VAR}")

# Link against GTest and ${PROJECT_NAME} library
target_link_libraries(${TEST_TARGET_NAME} PRIVATE  ${PROJECT_NAME} GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

# Register test with CTest
add_test(NAME ${TEST_TARGET_NAME} COMMAND ${TEST_TARGET_NAME})