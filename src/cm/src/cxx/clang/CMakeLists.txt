
# Configuting clang library
find_package(Clang REQUIRED CONFIG)

# C++ Source Code model builder from clang AST
add_library(cm-src-cxx-clang
            ast_converter.cpp
            cmsrcclang.cpp
           )

target_precompile_headers(cm-src-cxx-clang PRIVATE pch.hpp)
target_link_libraries(cm-src-cxx-clang PUBLIC cm-src-cxx cm-cxx-clang)
target_include_directories(cm-src-cxx-clang PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

set(clang_libraries
    libclang
    clangAST
   )

target_link_libraries(cm-src-cxx-clang PRIVATE ${clang_libraries})

add_subdirectory(test)

if("${CM_ENABLE_TOOLS}")
    add_executable(cm-src-cxx-clang-print cmsrcclangprint.cpp)
    target_link_libraries(cm-src-cxx-clang-print PRIVATE cm-src-cxx-clang Boost::program_options)
    target_precompile_headers(cm-src-cxx-clang-print PRIVATE pch.hpp)
endif()
