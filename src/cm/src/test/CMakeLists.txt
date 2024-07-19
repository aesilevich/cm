
# Source code model test
add_executable(cm-src-test
               source_code_model_test.cpp
               source_file_position_test.cpp
               test.cpp
              )

target_precompile_headers(cm-src-test PRIVATE "../pch.hpp")
target_link_libraries(cm-src-test PRIVATE cm-src Boost::unit_test_framework)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cm-src-test PRIVATE rt)
endif()

add_test(NAME cm-src-test COMMAND cm-src-test)
