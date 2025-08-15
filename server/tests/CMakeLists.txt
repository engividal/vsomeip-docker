cmake_minimum_required(VERSION 3.10)
project(vsomeip_tests)

# Enable C++11 or higher
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Find required packages
find_package(Boost REQUIRED COMPONENTS system thread log)
find_package(vsomeip3 REQUIRED)

# Coverage flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
    option(ENABLE_COVERAGE "Enable coverage reporting" ON)
    if(ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -g -O0")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
endif()

# Set Google Test paths
set(GTEST_ROOT /usr/src/gtest)
set(GTEST_INCLUDE_DIRS ${GTEST_ROOT}/include)
set(GTEST_LIBRARIES ${GTEST_ROOT}/lib/libgtest.a)
set(GTEST_MAIN_LIBRARIES ${GTEST_ROOT}/lib/libgtest_main.a)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${VSOMEIP_INCLUDE_DIRS})

# Add executable for deserialization tests
add_executable(runDeserializationTests test_server.cpp ../sensor_data.cpp)
target_link_libraries(runDeserializationTests 
    ${GTEST_MAIN_LIBRARIES} ${GTEST_LIBRARIES}
    ${Boost_LIBRARIES} vsomeip3 vsomeip3-cfg vsomeip3-sd
    pthread)

# Add executable for handler tests  
add_executable(runHandlerTests test_server_handlers.cpp ../sensor_data.cpp)
target_link_libraries(runHandlerTests 
    ${GTEST_MAIN_LIBRARIES} ${GTEST_LIBRARIES}
    ${Boost_LIBRARIES} vsomeip3 vsomeip3-cfg vsomeip3-sd
    pthread)

# Add executable for all tests combined
add_executable(runAllTests test_server.cpp test_server_handlers.cpp ../sensor_data.cpp)
target_link_libraries(runAllTests 
    ${GTEST_MAIN_LIBRARIES} ${GTEST_LIBRARIES}
    ${Boost_LIBRARIES} vsomeip3 vsomeip3-cfg vsomeip3-sd
    pthread)

# Add tests to CTest
add_test(NAME DeserializationTests COMMAND runDeserializationTests)
add_test(NAME HandlerTests COMMAND runHandlerTests)
add_test(NAME AllTests COMMAND runAllTests)

# Custom target for coverage report (requires lcov)
if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory coverage
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage/coverage.info
            COMMAND ${LCOV_PATH} --remove coverage/coverage.info '/usr/*' --output-file coverage/coverage.info
            COMMAND ${LCOV_PATH} --remove coverage/coverage.info '*/gtest/*' --output-file coverage/coverage.info
            COMMAND ${GENHTML_PATH} coverage/coverage.info --output-directory coverage/html
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated in coverage/html/index.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS runAllTests
            COMMENT "Generating code coverage report"
        )
    endif()
endif()
