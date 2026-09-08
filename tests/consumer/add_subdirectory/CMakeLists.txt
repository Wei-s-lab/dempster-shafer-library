cmake_minimum_required(VERSION 3.16)
project(ds_consumer_add_subdirectory LANGUAGES CXX)

# DS_ROOT must point at the Dempster-Shafer library source tree.
if(NOT DEFINED DS_ROOT OR DS_ROOT STREQUAL "")
    message(FATAL_ERROR "Pass -DDS_ROOT=/path/to/Dempster-Shafer-library")
endif()

add_subdirectory("${DS_ROOT}" "${CMAKE_BINARY_DIR}/_ds")

# Consuming via add_subdirectory must not pull in the demo executable.
if(TARGET ds_demo)
    message(FATAL_ERROR
        "ds_demo should not exist when this library is consumed via "
        "add_subdirectory (DS_BUILD_EXAMPLES should default to OFF)")
endif()

if(NOT TARGET ds::ds)
    message(FATAL_ERROR "Expected INTERFACE alias target ds::ds after add_subdirectory")
endif()

add_executable(consumer_main main.cpp)
target_link_libraries(consumer_main PRIVATE ds::ds)
