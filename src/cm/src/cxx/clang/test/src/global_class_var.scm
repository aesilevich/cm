source_file "global_class_var.cpp" {
    decl_context {
        namespace_decl ["global_class_var.cpp": (2, 1) - (4, 1)] {
            identifier ["global_class_var.cpp": (2, 11) - (2, 13)] ns;
            decl_context {
                record ["global_class_var.cpp": (3, 5) - (3, 21)] {
                    identifier ["global_class_var.cpp": (3, 11) - (3, 19)] my_class;
                    decl_context {
                    }
                }
            }
        }
        global_variable_decl ["global_class_var.cpp": (6, 1) - (6, 14)] {
            identifier ["global_class_var.cpp": (6, 14) - (6, 15)] x;
            record_type_spec ["global_class_var.cpp": (6, 1) - (6, 13)] {
                namespace_scope_spec ["global_class_var.cpp": (6, 1) - (6, 3)] {
                    identifier ["global_class_var.cpp": (6, 1) - (6, 3)] ns;
                }
                identifier ["global_class_var.cpp": (6, 5) - (6, 13)] my_class;
            }
        }
    }
}
