source_file "class_static_func.cpp" {
    decl_context {
        record ["class_static_func.cpp": (4, 1) - (7, 1)] {
            identifier ["class_static_func.cpp": (4, 7) - (4, 15)] my_class;
            decl_context {
                static_method_decl ["class_static_func.cpp": (6, 5) - (6, 26)] {
                    identifier ["class_static_func.cpp": (6, 17) - (6, 20)] foo;
                    builtin_type_spec ["class_static_func.cpp": (6, 12) - (6, 12)] void;
                    function_parameter_decl ["class_static_func.cpp": (6, 21) - (6, 25)] {
                        identifier ["class_static_func.cpp": (6, 25) - (6, 26)] x;
                        builtin_type_spec ["class_static_func.cpp": (6, 21) - (6, 21)] int;
                    }
                }
            }
        }
    }
}
