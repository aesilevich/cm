
# Code Model library project

cmake_minimum_required(VERSION 3.6)
project(cm)

include(CTest)

option(CM_ENABLE_SRC "Enable source code model" ON)
option(CM_ENABLE_TOOLS "Enable code model command line tools" ON)

option(CM_ENABLE_CXX "Enable C++ model" ON)
option(CM_CXX_ENABLE_CLANG "Enable Clang parser for C++ code model" ON)

set(CMAKE_CXX_STANDARD 20)


# Configuring Boost library

set(boost_libraries log)

# Boost.Test is required for building tests
if("${BUILD_TESTING}")
    list(APPEND boost_libraries unit_test_framework)
endif()

# Boost.program_options is required for command line tools
if("${CM_ENABLE_TOOLS}")
    list(APPEND boost_libraries program_options)
endif()

find_package(Boost 1.64 REQUIRED COMPONENTS ${boost_libraries})


set(CM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(src)
