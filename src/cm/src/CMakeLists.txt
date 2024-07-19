
# Source code model library
add_library(cm-src
            ast_printer.cpp
            declaration.cpp
            functional_decl.cpp
            source_code_model.cpp
            template_decl.cpp
           )

target_precompile_headers(cm-src PRIVATE pch.hpp)
target_link_libraries(cm-src PUBLIC cm)

add_subdirectory(test)
add_subdirectory(cxx)
