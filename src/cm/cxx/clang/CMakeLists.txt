
# Configuting clang library
find_package(Clang REQUIRED CONFIG)

# C++ Code model builder from clang AST
add_library(cm-cxx-clang
            array_type_converter.hpp
            ast_converter.cpp
            builtin_type_converter.cpp
            builtin_type_converter.hpp
            cmclang.cpp
            converter_impl.cpp
            converter_impl.hpp
            convert_function.cpp
            convert_function.hpp
            convert_loc.cpp
            convert_loc.hpp
            convert_record.cpp
            convert_record.hpp
            convert_template.cpp
            convert_template.hpp
            decl_map.cpp
            decltype_type_converter.cpp
            decltype_type_converter.hpp
            dependent_type_converter.cpp
            dependent_type_converter.hpp
            function_converter.cpp
            function_converter.hpp
            function_type_converter.cpp
            function_type_converter.hpp
            import.hpp
            linkage_spec_converter.cpp
            linkage_spec_converter.hpp
            lvalue_reference_type_converter.hpp
            namespace_converter.cpp
            namespace_converter.hpp
            pch.hpp
            pack_expansion_type_converter.hpp
            pointer_type_converter.hpp
            record_converter.cpp
            record_converter.hpp
            rvalue_reference_type_converter.hpp
            template_function_converter.cpp
            template_function_converter.hpp
            template_record_converter.cpp
            template_record_converter.hpp
            template_record_partial_specialization_converter.cpp
            template_record_partial_specialization_converter.hpp
            template_record_specialization_converter.cpp
            template_record_specialization_converter.hpp
            template_specialization_type_converter.cpp
            template_specialization_type_converter.hpp
            type_converter.hpp
            type_template_parameter_converter.cpp
            type_template_parameter_converter.hpp
            typedef_converter.cpp
            typedef_converter.hpp
            types_converter.hpp
            variable_converter.cpp
            variable_converter.hpp
            utils.cpp
            utils.hpp
           )

target_precompile_headers(cm-cxx-clang PRIVATE pch.hpp)
target_link_libraries(cm-cxx-clang PUBLIC cm-cxx cm-log)
target_include_directories(cm-cxx-clang PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

set(clang_libraries
    libclang
    clangAST
    clangBasic
    clangLex
    LLVMSupport
   )

target_link_libraries(cm-cxx-clang PRIVATE ${clang_libraries})

add_subdirectory(test)

if("${CM_ENABLE_TOOLS}")
    add_executable(cm-cxx-clang-dump cmclangdump.cpp)
    target_link_libraries(cm-cxx-clang-dump PUBLIC cm-cxx-clang
                          Boost::program_options
                          ${CM_BOOST_HEADERS_LIBRARIES})
    target_precompile_headers(cm-cxx-clang-dump PRIVATE pch.hpp)
endif()
