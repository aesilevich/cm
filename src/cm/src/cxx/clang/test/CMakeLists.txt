

# Code model clang builder test
add_executable(cm-src-cxx-clang-test
               parse_fixture.hpp
               parse_simple_tests.cpp
               test.cpp
              )

target_link_libraries(cm-src-cxx-clang-test PRIVATE cm-src-cxx-clang Boost::unit_test_framework)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cm-src-cxx-clang-test PRIVATE rt)
endif()

add_test(NAME cm-src-cxx-clang-test COMMAND cm-src-cxx-clang-test)


# Parse tests with the cm-src-clang-print utility
set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE parse_tests RELATIVE "${src_dir}" "${src_dir}/*.cpp")
foreach(file ${parse_tests})
    get_filename_component(file_name "${file}" NAME_WE)
    add_test(NAME "cm-src-cxx-parse-${file_name}"
             COMMAND cm-src-cxx-clang-print "${src_dir}/${file_name}.cpp"
                                            "--compare" "${src_dir}/${file_name}.scm")
endforeach()
