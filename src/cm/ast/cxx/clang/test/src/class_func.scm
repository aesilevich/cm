source_file "class_func.cpp" {
    decl_context {
        record ["class_func.cpp": (4, 1) - (7, 1)] {
            identifier ["class_func.cpp": (4, 7) - (4, 15)] my_class;
            decl_context {
                method_decl ["class_func.cpp": (6, 5) - (6, 31)] {
                    identifier ["class_func.cpp": (6, 11) - (6, 14)] foo;
                    builtin_type_spec ["class_func.cpp": (6, 5) - (6, 5)] float;
                    function_parameter_decl ["class_func.cpp": (6, 15) - (6, 28)] {
                        identifier ["class_func.cpp": (6, 28) - (6, 31)] par;
                        builtin_type_spec ["class_func.cpp": (6, 15) - (6, 19)] unsigned int;
                    }
                }
            }
        }
        method_decl ["class_func.cpp": (9, 1) - (12, 1)] {
            identifier ["class_func.cpp": (9, 17) - (9, 20)] foo;
            record_scope_spec ["class_func.cpp": (9, 7) - (9, 15)] {
                identifier ["class_func.cpp": (9, 7) - (9, 15)] my_class;
            }
            builtin_type_spec ["class_func.cpp": (9, 1) - (9, 1)] float;
            function_parameter_decl ["class_func.cpp": (9, 21) - (11, 1)] {
                identifier ["class_func.cpp": (11, 1) - (11, 4)] par;
                builtin_type_spec ["class_func.cpp": (9, 21) - (10, 1)] unsigned int;
            }
            opaque {
            }
        }
    }
}
