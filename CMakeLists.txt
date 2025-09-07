cmake_minimum_required(VERSION 3.10)
project(Scanner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Werror -O3")



add_library(logger SHARED src/logger.cpp)
set_target_properties(logger PROPERTIES
OUTPUT_NAME "logger"
PREFIX ""
SUFFIX ".dll"
)
target_link_libraries(logger)


add_library(csvloader SHARED src/csvloader.cpp)
set_target_properties(csvloader PROPERTIES
OUTPUT_NAME "csvloader"
PREFIX ""
SUFFIX ".dll"
)
target_link_libraries(csvloader logger)

add_library(hasher SHARED src/hasher.cpp)
set_target_properties(hasher PROPERTIES
OUTPUT_NAME "hasher"
PREFIX ""
SUFFIX ".dll"
)
target_link_libraries(hasher logger)

add_library(walker SHARED src/walker.cpp)
set_target_properties(walker PROPERTIES
OUTPUT_NAME "walker"
PREFIX ""
SUFFIX ".dll"
)
target_link_libraries(walker hasher logger)


add_executable(main_exe src/main.cpp)
set_target_properties(main_exe PROPERTIES
OUTPUT_NAME "main"
# RUNTIME_OUTPUT_DIRECTORY "scanner"
)

target_link_libraries(main_exe csvloader walker hasher logger)






set(GTEST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/googletest/include/)
set(GTEST_LIB_DIR ${CMAKE_SOURCE_DIR}/thirdparty/googletest/lib)

include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIB_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src/)

add_executable(logger_test tests/logger_test.cpp)
set_target_properties(logger_test PROPERTIES
    OUTPUT_NAME "logger_test"
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
target_link_libraries(logger_test logger gtest)

add_executable(hasher_test tests/hasher_test.cpp)
set_target_properties(hasher_test PROPERTIES
    OUTPUT_NAME "hasher_test"
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
target_link_libraries(hasher_test hasher logger gtest)

add_executable(csvloader_test tests/csvloader_test.cpp)
set_target_properties(csvloader_test PROPERTIES
    OUTPUT_NAME "csvloader_test"
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
target_link_libraries(csvloader_test csvloader logger gtest)

add_executable(walker_test tests/walker_test.cpp)
set_target_properties(walker_test PROPERTIES
    OUTPUT_NAME "walker_test"
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
target_link_libraries(walker_test walker hasher logger gtest)


enable_testing()
add_test(LogTest logger_test)
add_test(HashTest hasher_test)
add_test(CsvTest csvloader_test)
add_test(WalkTest walker_test)

add_custom_target(run_tests
    COMMAND ctest -V
    DEPENDS logger_test hasher_test csvloader_test walker_test # Replace with your test target name if different
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests with CTest..."
)