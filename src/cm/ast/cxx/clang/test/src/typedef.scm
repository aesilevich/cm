source_file "typedef.cpp" {
    decl_context {
        typedef ["typedef.cpp": (4, 1) - (4, 36)] {
            identifier ["typedef.cpp": (4, 15) - (4, 22)] my_func;
            parens_type_spec ["typedef.cpp": (4, 9) - (4, 36)] {
                function_type_spec ["typedef.cpp": (4, 9) - (4, 36)] {
                    builtin_type_spec ["typedef.cpp": (4, 9) - (4, 9)] void;
                    function_type_spec_parameter {
                        identifier ["typedef.cpp": (4, 28) - (4, 29)] x;
                        builtin_type_spec ["typedef.cpp": (4, 24) - (4, 24)] int;
                    }
                    function_type_spec_parameter {
                        identifier ["typedef.cpp": (4, 35) - (4, 36)] y;
                        builtin_type_spec ["typedef.cpp": (4, 31) - (4, 31)] int;
                    }
                }
            }
        }
        typedef ["typedef.cpp": (5, 1) - (6, 12)] {
            identifier ["typedef.cpp": (6, 5) - (6, 12)] my_type;
            builtin_type_spec ["typedef.cpp": (5, 9) - (5, 9)] int;
        }
    }
}
