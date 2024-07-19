source_file "namespace.cpp" {
    decl_context {
        namespace_decl ["namespace.cpp": (4, 1) - (6, 1)] {
            identifier ["namespace.cpp": (4, 11) - (4, 14)] foo;
            decl_context {
                namespace_decl ["namespace.cpp": (4, 14) - (6, 1)] {
                    identifier ["namespace.cpp": (4, 16) - (4, 19)] bar;
                    decl_context {
                    }
                }
            }
        }
    }
}
