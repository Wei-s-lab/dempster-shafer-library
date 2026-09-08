cmake_minimum_required(VERSION 3.16)

project(dempster_shafer
    VERSION 0.1.0
    DESCRIPTION "Dempster-Shafer evidential-theory library"
    LANGUAGES CXX)

# Header-only INTERFACE library. Downstream projects link ds::ds and inherit
# the include path and cxx_std_17 requirement; there is nothing to compile here.
add_library(ds INTERFACE)
add_library(ds::ds ALIAS ds)

target_include_directories(ds INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(ds INTERFACE cxx_std_17)

# When this repo is the top-level project, build examples/tests by default.
# When another project add_subdirectory()'s us, both default to OFF so
# consumers do not pull in examples/demo.cpp or the unit-test binaries.
# Override at configure time with -DDS_BUILD_EXAMPLES=ON/OFF etc.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(DS_IS_TOP_LEVEL ON)
else()
    set(DS_IS_TOP_LEVEL OFF)
endif()

option(DS_BUILD_EXAMPLES "Build the Dempster-Shafer example programs" ${DS_IS_TOP_LEVEL})
option(DS_BUILD_TESTS "Build the Dempster-Shafer tests" ${DS_IS_TOP_LEVEL})

if(DS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(DS_BUILD_TESTS)
    # include(CTest) enables CTest, defines BUILD_TESTING, and wires ctest.
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

# Install headers + CMake package config only (no examples/tests artifacts).
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS ds
    EXPORT dsTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT dsTargets
    FILE dsTargets.cmake
    NAMESPACE ds::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ds)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/dsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ds)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/dsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/dsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ds)
