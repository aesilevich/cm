

# C++ code model clang builder test
add_executable(cm-cxx-clang-test
               import.hpp
               parse_fixture.hpp
               parse_simple_tests.cpp
               pch.hpp
               test.cpp
              )

target_precompile_headers(cm-cxx-clang-test PRIVATE pch.hpp)
target_link_libraries(cm-cxx-clang-test PRIVATE cm-cxx-clang Boost::unit_test_framework)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cm-cxx-clang-test PRIVATE rt)
endif()

add_test(NAME cm-cxx-clang-test COMMAND cm-cxx-clang-test)


# Parse tests with the cmclangdump utility
set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE parse_tests RELATIVE "${src_dir}" "${src_dir}/*.cpp")
foreach(file ${parse_tests})
    get_filename_component(file_name "${file}" NAME_WE)
    add_test(NAME "cm-cxx-clang-parse-${file_name}"
             COMMAND cm-cxx-clang-dump "${src_dir}/${file_name}.cpp"
                                       "--compare" "${src_dir}/${file_name}.cm")
endforeach()
