source_file "template_func.cpp" {
    decl_context {
        template ["template_func.cpp": (2, 1) - (3, 13)] {
            type_template_parameter_decl ["template_func.cpp": (2, 11) - (2, 21)] {
                identifier ["template_func.cpp": (2, 20) - (2, 21)] A;
            }
            value_template_parameter_decl ["template_func.cpp": (2, 23) - (2, 26)] {
                identifier ["template_func.cpp": (2, 25) - (2, 26)] y;
                template_param_type_spec ["template_func.cpp": (2, 23) - (2, 24)] {
                    identifier ["template_func.cpp": (2, 23) - (2, 24)] A;
                }
            }
            function_decl ["template_func.cpp": (3, 1) - (3, 13)] {
                identifier ["template_func.cpp": (3, 6) - (3, 9)] foo;
                builtin_type_spec ["template_func.cpp": (3, 1) - (3, 1)] void;
                function_parameter_decl ["template_func.cpp": (3, 10) - (3, 12)] {
                    identifier ["template_func.cpp": (3, 12) - (3, 13)] a;
                    template_param_type_spec ["template_func.cpp": (3, 10) - (3, 11)] {
                        identifier ["template_func.cpp": (3, 10) - (3, 11)] A;
                    }
                }
            }
        }
        template ["template_func.cpp": (5, 1) - (8, 1)] {
            type_template_parameter_decl ["template_func.cpp": (5, 11) - (5, 21)] {
                identifier ["template_func.cpp": (5, 20) - (5, 21)] T;
            }
            value_template_parameter_decl ["template_func.cpp": (5, 23) - (5, 26)] {
                identifier ["template_func.cpp": (5, 25) - (5, 26)] x;
                template_param_type_spec ["template_func.cpp": (5, 23) - (5, 24)] {
                    identifier ["template_func.cpp": (5, 23) - (5, 24)] T;
                }
            }
            function_decl ["template_func.cpp": (6, 1) - (8, 1)] {
                identifier ["template_func.cpp": (6, 6) - (6, 9)] foo;
                builtin_type_spec ["template_func.cpp": (6, 1) - (6, 1)] void;
                function_parameter_decl ["template_func.cpp": (6, 10) - (6, 12)] {
                    identifier ["template_func.cpp": (6, 12) - (6, 15)] zzz;
                    template_param_type_spec ["template_func.cpp": (6, 10) - (6, 11)] {
                        identifier ["template_func.cpp": (6, 10) - (6, 11)] T;
                    }
                }
                opaque {
                }
            }
        }
        function_decl ["template_func.cpp": (10, 1) - (12, 1)] {
            identifier ["template_func.cpp": (10, 6) - (10, 9)] bar;
            builtin_type_spec ["template_func.cpp": (10, 1) - (10, 1)] void;
            opaque {
                builtin_type_spec ["template_func.cpp": (11, 9) - (11, 9)] int;
            }
        }
    }
}
