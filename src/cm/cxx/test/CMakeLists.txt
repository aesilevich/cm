
if(NOT "${BUILD_TESTING}")
    return()
endif()

# C++ code model test
add_executable(cm-cxx-test
               cxx_print_test.cpp
               test.cpp
              )

target_precompile_headers(cm-cxx-test PRIVATE pch.hpp)
target_link_libraries(cm-cxx-test PRIVATE cm-cxx Boost::unit_test_framework)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cm-cxx-test PRIVATE rt)
endif()

add_test(NAME cm-test COMMAND cm-cxx-test)
