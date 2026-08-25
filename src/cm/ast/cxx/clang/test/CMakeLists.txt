

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
file(GLOB parse_tests RELATIVE "${src_dir}" "${src_dir}/*.cpp")
foreach(file ${parse_tests})
    get_filename_component(file_name "${file}" NAME_WE)
    add_test(NAME "cm-ast-cxx-parse-${file_name}"
             COMMAND cm-ast-cxx-clang-print "${src_dir}/${file_name}.cpp"
                                            "--compare" "${src_dir}/${file_name}.cmast")
endforeach()


# Parse tests for sources of the code model test suite. Sources are taken from the
# code model test suite, expected results are stored in the cm subdirectory.
#
# Tests in this list are not converted by the AST converter yet. Each one is
# disabled because of an assert in the AST converter, not because of a wrong result.
set(cm_disabled_parse_tests
        decltype_type                       # DecltypeTypeLoc is not implemented
        dependent_name                      # type loc conversion without function body
        partial_template_specialization     # QualifiedTypeLoc is not implemented
        template_arg_decl                   # QualifiedTypeLoc is not implemented
        template_arg_expr                   # type loc conversion without function body
        template_arg_template               # template template parameters are not implemented
        template_arg_template_expansion     # template template parameters are not implemented
        template_class_specialization       # template specialization parsing is not implemented
        template_dependent_inst             # dependent template instantiation is not implemented
        template_param_template             # template template parameters are not implemented
        typedef                             # QualifiedTypeLoc is not implemented
        typedef_record                      # QualifiedTypeLoc is not implemented
        var                                 # QualifiedTypeLoc is not implemented
        variadic_func                       # PackExpansionTypeLoc is not implemented
        variadic_templ_arg_pack             # type loc conversion without function body
    )

set(cm_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cxx/clang/test/src")
set(cm_result_dir "${src_dir}/cm")

file(GLOB cm_parse_tests RELATIVE "${cm_src_dir}" "${cm_src_dir}/*.cpp")
foreach(file ${cm_parse_tests})
    get_filename_component(file_name "${file}" NAME_WE)

    if("${file_name}" IN_LIST cm_disabled_parse_tests)
        continue()
    endif()

    add_test(NAME "cm-ast-cxx-parse-cm-${file_name}"
             COMMAND cm-ast-cxx-clang-print "${cm_src_dir}/${file_name}.cpp"
                                            "--compare" "${cm_result_dir}/${file_name}.cmast"
                                            "--" "-std=c++20")
endforeach()
