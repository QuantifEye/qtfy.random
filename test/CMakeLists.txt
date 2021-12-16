cmake_minimum_required(VERSION ${QTFY_CMAKE_MINIMUM_VERSION})

function(qtfy_add_test target_name)
    add_executable(${target_name} ${ARGN})
    target_link_libraries(
            ${target_name}
            PUBLIC
            project_options
            project_warnings
            qtfy_interface
    )
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

qtfy_add_test(counter_tests counter_tests.cpp)
qtfy_add_test(philox_trait_tests philox_trait_tests.cpp)
qtfy_add_test(threefry_tests threefry_tests.cpp)
qtfy_add_test(counter_based_generator_tests counter_based_generator_tests.cpp)
qtfy_add_test(utility_tests utility_tests.cpp)