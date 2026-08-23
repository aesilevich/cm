
# Source code model library
add_library(cm-ast
            ast_printer.cpp
            declaration.cpp
            functional_decl.cpp
            ast_model.cpp
            template_decl.cpp
           )

target_precompile_headers(cm-ast PRIVATE pch.hpp)
target_link_libraries(cm-ast PUBLIC cm)

add_subdirectory(test)
add_subdirectory(cxx)
