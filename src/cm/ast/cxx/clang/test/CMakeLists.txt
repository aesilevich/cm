

# Code model clang builder test
add_executable(cm-ast-cxx-clang-test
               parse_fixture.hpp
               parse_simple_tests.cpp
               test.cpp
              )

target_link_libraries(cm-ast-cxx-clang-test PRIVATE
                      cm-ast-cxx-clang
                      ${CM_BOOST_HEADERS_LIBRARIES})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cm-ast-cxx-clang-test PRIVATE rt)
endif()

add_test(NAME cm-ast-cxx-clang-test COMMAND cm-ast-cxx-clang-test)


# Parse tests with the cm-ast-clang-print utility
set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE parse_tests RELATIVE "${src_dir}" "${src_dir}/*.cpp")
foreach(file ${parse_tests})
    get_filename_component(file_name "${file}" NAME_WE)
    add_test(NAME "cm-ast-cxx-parse-${file_name}"
             COMMAND cm-ast-cxx-clang-print "${src_dir}/${file_name}.cpp"
                                            "--compare" "${src_dir}/${file_name}.cmast")
endforeach()
