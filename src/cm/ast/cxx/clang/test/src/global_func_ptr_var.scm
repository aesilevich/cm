source_file "global_func_ptr_var.cpp" {
    decl_context {
        global_variable_decl ["global_func_ptr_var.cpp": (2, 1) - (2, 24)] {
            identifier ["global_func_ptr_var.cpp": (2, 7) - (2, 10)] var;
            pointer_type_spec ["global_func_ptr_var.cpp": (2, 1) - (2, 24)] {
                parens_type_spec ["global_func_ptr_var.cpp": (2, 1) - (2, 24)] {
                    function_type_spec ["global_func_ptr_var.cpp": (2, 1) - (2, 24)] {
                        builtin_type_spec ["global_func_ptr_var.cpp": (2, 1) - (2, 1)] int;
                        function_type_spec_parameter {
                            identifier ["global_func_ptr_var.cpp": (2, 16) - (2, 17)] x;
                            builtin_type_spec ["global_func_ptr_var.cpp": (2, 12) - (2, 12)] int;
                        }
                        function_type_spec_parameter {
                            identifier ["global_func_ptr_var.cpp": (2, 23) - (2, 24)] y;
                            builtin_type_spec ["global_func_ptr_var.cpp": (2, 19) - (2, 19)] int;
                        }
                    }
                }
            }
        }
    }
}
