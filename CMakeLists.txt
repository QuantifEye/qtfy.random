set(QTFY_CMAKE_MINIMUM_VERSION 3.15)
cmake_minimum_required(VERSION ${QTFY_CMAKE_MINIMUM_VERSION})

project(Quantifeye CXX)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)

function(qtfy_add_test target_name)
    add_executable(${target_name} ${ARGN})
    target_link_libraries(
            ${target_name}
            PUBLIC
            project_options
            project_warnings
            qtfy_interface)
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

enable_testing()

add_subdirectory(include)
add_subdirectory(examples)
add_subdirectory(test)


