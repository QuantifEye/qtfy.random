cmake_minimum_required(VERSION ${QTFY_CMAKE_MINIMUM_VERSION})



qtfy_add_test(counter_tests counter_tests.cpp)
qtfy_add_test(philox_trait_tests philox_trait_tests.cpp)
qtfy_add_test(threefry_tests threefry_tests.cpp)
qtfy_add_test(counter_based_generator_tests counter_based_generator_tests.cpp)
