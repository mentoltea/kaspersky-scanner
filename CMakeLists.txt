cmake_minimum_required(VERSION 3.10)
project(Scanner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Werror -ggdb")

add_library(logger SHARED src/logger.cpp)
set_target_properties(logger PROPERTIES
    OUTPUT_NAME "logger"
    PREFIX ""
    SUFFIX ".dll"
)


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
)

target_link_libraries(main_exe csvloader walker hasher logger)