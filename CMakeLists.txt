cmake_minimum_required(VERSION 3.17)
project(random)

set(CMAKE_CXX_STANDARD 23)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)

add_executable(random tests.cpp random.h)