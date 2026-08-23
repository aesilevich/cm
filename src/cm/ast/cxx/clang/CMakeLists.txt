
# Configuting clang library
find_package(Clang REQUIRED CONFIG)

# C++ Source Code model builder from clang AST
add_library(cm-ast-cxx-clang
            ast_converter.cpp
            cmastclang.cpp
           )

target_precompile_headers(cm-ast-cxx-clang PRIVATE pch.hpp)
target_link_libraries(cm-ast-cxx-clang PUBLIC cm-ast-cxx cm-cxx-clang)
target_include_directories(cm-ast-cxx-clang PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

set(clang_libraries
    libclang
    clangAST
    clangBasic
    clangLex
    LLVMSupport
   )

target_link_libraries(cm-ast-cxx-clang PRIVATE ${clang_libraries})

add_subdirectory(test)

if("${CM_ENABLE_TOOLS}")
    add_executable(cm-ast-cxx-clang-print cmastclangprint.cpp)
    target_link_libraries(cm-ast-cxx-clang-print PRIVATE
                          cm-ast-cxx-clang
                          Boost::program_options
                          ${CM_BOOST_HEADERS_LIBRARIES})
    target_precompile_headers(cm-ast-cxx-clang-print PRIVATE pch.hpp)
endif()
