
# Source code model test
add_executable(cm-ast-test
               ast_model_test.cpp
               source_file_position_test.cpp
               test.cpp
              )

target_precompile_headers(cm-ast-test PRIVATE "../pch.hpp")
target_link_libraries(cm-ast-test PRIVATE
                      cm-ast
                      ${CM_BOOST_HEADERS_LIBRARIES})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cm-ast-test PRIVATE rt)
endif()

add_test(NAME cm-ast-test COMMAND cm-ast-test)
