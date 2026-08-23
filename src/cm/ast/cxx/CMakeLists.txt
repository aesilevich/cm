
# Source code model library
add_library(cm-ast-cxx
            cm-ast-cxx.cpp
           )

target_precompile_headers(cm-ast-cxx PRIVATE pch.hpp)
target_link_libraries(cm-ast-cxx PUBLIC cm-ast cm-cxx)

#add_subdirectory(test)
add_subdirectory(clang)
