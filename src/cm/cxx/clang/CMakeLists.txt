
# Configuting clang library
find_package(Clang REQUIRED CONFIG)

# C++ Code model builder from clang AST
add_library(cm-cxx-clang
            ast_converter.cpp
            cmclang.cpp
            import.hpp
            pch.hpp
           )

target_precompile_headers(cm-cxx-clang PRIVATE pch.hpp)
target_link_libraries(cm-cxx-clang PUBLIC cm-cxx)
target_include_directories(cm-cxx-clang PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

set(clang_libraries
    libclang
    clangAST
   )

target_link_libraries(cm-cxx-clang PRIVATE ${clang_libraries})

add_subdirectory(test)

if("${CM_ENABLE_TOOLS}")
    add_executable(cm-cxx-clang-dump cmclangdump.cpp)
    target_link_libraries(cm-cxx-clang-dump PUBLIC cm-cxx-clang Boost::program_options)
    target_precompile_headers(cm-cxx-clang-dump PRIVATE pch.hpp)
endif()
