
add_library(cm-cxx
            print.cpp)

target_link_libraries(cm-cxx PUBLIC cm)

if("${CM_CXX_ENABLE_CLANG}")
    add_subdirectory(clang)
endif()
