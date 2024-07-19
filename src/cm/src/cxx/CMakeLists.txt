
# Source code model library
add_library(cm-src-cxx
            cm-src-cxx.cpp
           )

target_precompile_headers(cm-src-cxx PRIVATE pch.hpp)
target_link_libraries(cm-src-cxx PUBLIC cm-src cm-cxx)

#add_subdirectory(test)
add_subdirectory(clang)
