
add_library(cm-cxx
            print.cpp)

target_link_libraries(cm-cxx PUBLIC cm)

add_subdirectory(test)

if("${CM_CXX_ENABLE_CLANG}")
    add_subdirectory(clang)
endif()
