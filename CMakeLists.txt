set(QTFY_CMAKE_MINIMUM_VERSION 3.15)
cmake_minimum_required(VERSION ${QTFY_CMAKE_MINIMUM_VERSION})

project(Quantifeye CXX)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

add_subdirectory(include)

option(ENABLE_TESTING "Enable Test Builds" ON)
if (ENABLE_TESTING)
    enable_testing()
    message("** Testing Enabled")
    add_subdirectory(test)
endif ()

