
if(NOT "${BUILD_TESTING}")
    return()
endif()

# Code model test
add_executable(cm-test
               builder_test.cpp
               code_model_test.cpp
               context_test.cpp
               debug_info_test.cpp
               find_field_test.cpp
               test.cpp
              )

target_precompile_headers(cm-test PRIVATE pch.hpp)
target_link_libraries(cm-test PRIVATE cm Boost::unit_test_framework)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cm-test PRIVATE rt)
endif()

add_test(NAME cm-test COMMAND cm-test)
