source_file "global_rref_var.cpp" {
    decl_context {
        global_variable_decl ["global_rref_var.cpp": (2, 1) - (2, 12)] {
            identifier ["global_rref_var.cpp": (2, 5) - (2, 9)] base;
            builtin_type_spec ["global_rref_var.cpp": (2, 1) - (2, 1)] int;
        }
        global_variable_decl ["global_rref_var.cpp": (3, 1) - (3, 37)] {
            identifier ["global_rref_var.cpp": (3, 8) - (3, 11)] var;
            rvalue_reference_type_spec ["global_rref_var.cpp": (3, 1) - (3, 5)] {
                builtin_type_spec ["global_rref_var.cpp": (3, 1) - (3, 1)] int;
            }
        }
    }
}
