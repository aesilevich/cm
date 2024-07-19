
# Code model library
add_library(cm
            builder.cpp
            code_model.cpp
            debug_info.cpp
            context_entity.cpp
            context.cpp
            find_field.cpp
            function.cpp
            named_entity.cpp
            named_type.cpp
            namespace.cpp
            ptr_or_ref_type.cpp
            qual_type.cpp
            record_type.cpp
            template_record.cpp
            type.cpp
            typedef_type.cpp
           )

target_include_directories(cm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
target_precompile_headers(cm PRIVATE pch.hpp)
add_subdirectory(test)

if("${CM_ENABLE_SRC}")
    add_subdirectory(src)
endif()

if("${CM_ENABLE_CXX}")
    add_subdirectory(cxx)
endif()
