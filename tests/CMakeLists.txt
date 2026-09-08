# CTest-driven executables. Each binary is one CTest case: exit 0 = pass.
# Run from the build tree with: ctest --output-on-failure

function(ds_add_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE ds::ds)
    # test_utils.hpp lives next to the sources; keep checks active in Release.
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES
        LABELS "ds"
        TIMEOUT 30)
endfunction()

ds_add_test(ds_frame_tests test_frame_of_discernment.cpp)
ds_add_test(ds_mass_tests test_mass_function.cpp)
ds_add_test(ds_combination_tests test_combination_rules.cpp)

# External-consumer smoke: add_subdirectory + install/find_package.
# Verifies ds::ds is usable and examples are not pulled into consumers.
set(_ds_smoke "${CMAKE_CURRENT_SOURCE_DIR}/consumer/run_smoke.sh")
add_test(
    NAME ds_consumer_smoke
    COMMAND "${CMAKE_COMMAND}" -E env
        "DS_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
        "DS_BINARY_DIR=${CMAKE_BINARY_DIR}"
        "CMAKE_COMMAND=${CMAKE_COMMAND}"
        "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        bash "${_ds_smoke}")
set_tests_properties(ds_consumer_smoke PROPERTIES
    LABELS "ds;consumer"
    TIMEOUT 120)
