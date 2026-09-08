cmake_minimum_required(VERSION 3.16)
project(ds_consumer_find_package LANGUAGES CXX)

find_package(ds 0.1 REQUIRED)

if(NOT TARGET ds::ds)
    message(FATAL_ERROR "find_package(ds) did not provide target ds::ds")
endif()

# Installed package must not expose the demo binary.
if(TARGET ds_demo)
    message(FATAL_ERROR "ds_demo must not appear after find_package(ds)")
endif()

add_executable(consumer_main main.cpp)
target_link_libraries(consumer_main PRIVATE ds::ds)
