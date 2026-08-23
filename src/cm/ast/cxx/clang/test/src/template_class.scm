source_file "template_class.cpp" {
    decl_context {
        template ["template_class.cpp": (4, 1) - (5, 7)] {
            type_template_parameter_decl ["template_class.cpp": (4, 11) - (4, 24)] {
                identifier ["template_class.cpp": (4, 20) - (4, 24)] TYPE;
            }
            value_template_parameter_decl ["template_class.cpp": (4, 26) - (4, 33)] {
                identifier ["template_class.cpp": (4, 30) - (4, 33)] NUM;
                builtin_type_spec ["template_class.cpp": (4, 26) - (4, 26)] int;
            }
            record ["template_class.cpp": (5, 1) - (5, 7)] {
                identifier ["template_class.cpp": (5, 7) - (5, 15)] my_class;
            }
        }
        template ["template_class.cpp": (7, 1) - (10, 1)] {
            type_template_parameter_decl ["template_class.cpp": (7, 11) - (7, 21)] {
                identifier ["template_class.cpp": (7, 20) - (7, 21)] T;
            }
            value_template_parameter_decl ["template_class.cpp": (7, 23) - (7, 28)] {
                identifier ["template_class.cpp": (7, 27) - (7, 28)] N;
                builtin_type_spec ["template_class.cpp": (7, 23) - (7, 23)] int;
            }
            record ["template_class.cpp": (8, 1) - (10, 1)] {
                identifier ["template_class.cpp": (8, 7) - (8, 15)] my_class;
                decl_context {
                    method_decl ["template_class.cpp": (9, 5) - (9, 17)] {
                        identifier ["template_class.cpp": (9, 10) - (9, 13)] foo;
                        builtin_type_spec ["template_class.cpp": (9, 5) - (9, 5)] void;
                        function_parameter_decl ["template_class.cpp": (9, 14) - (9, 16)] {
                            identifier ["template_class.cpp": (9, 16) - (9, 17)] x;
                            template_param_type_spec ["template_class.cpp": (9, 14) - (9, 15)] {
                                identifier ["template_class.cpp": (9, 14) - (9, 15)] T;
                            }
                        }
                    }
                }
            }
        }
        global_variable_decl ["template_class.cpp": (12, 1) - (13, 3)] {
            identifier ["template_class.cpp": (13, 3) - (13, 4)] x;
            template_record_instantiation_type_spec ["template_class.cpp": (12, 1) - (13, 1)] {
                template_spec ["template_class.cpp": (12, 1) - (12, 9)] {
                    identifier ["template_class.cpp": (12, 1) - (12, 9)] my_class;
                }
                type_template_argument_spec ["template_class.cpp": (12, 10) - (12, 13)] {
                    builtin_type_spec ["template_class.cpp": (12, 10) - (12, 10)] int;
                }
                value_template_argument_spec ["template_class.cpp": (12, 15) - (12, 17)] {
                }
            }
        }
    }
}
