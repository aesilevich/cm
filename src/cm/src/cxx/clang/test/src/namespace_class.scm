source_file "namespace_class.cpp" {
    decl_context {
        namespace_decl ["namespace_class.cpp": (2, 1) - (4, 1)] {
            identifier ["namespace_class.cpp": (2, 11) - (2, 13)] ns;
            decl_context {
                record ["namespace_class.cpp": (3, 5) - (3, 11)] {
                    identifier ["namespace_class.cpp": (3, 11) - (3, 19)] my_class;
                }
            }
        }
        record ["namespace_class.cpp": (6, 1) - (7, 1)] {
            identifier ["namespace_class.cpp": (6, 11) - (6, 19)] my_class;
            namespace_scope_spec ["namespace_class.cpp": (6, 7) - (6, 9)] {
                identifier ["namespace_class.cpp": (6, 7) - (6, 9)] ns;
            }
            decl_context {
            }
        }
    }
}
