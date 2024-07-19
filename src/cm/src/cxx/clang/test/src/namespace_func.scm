source_file "namespace_func.cpp" {
    decl_context {
        namespace_decl ["namespace_func.cpp": (4, 1) - (6, 1)] {
            identifier ["namespace_func.cpp": (4, 11) - (4, 13)] ns;
            decl_context {
                function_decl ["namespace_func.cpp": (5, 5) - (5, 27)] {
                    identifier ["namespace_func.cpp": (5, 11) - (5, 14)] foo;
                    builtin_type_spec ["namespace_func.cpp": (5, 5) - (5, 5)] float;
                    function_parameter_decl ["namespace_func.cpp": (5, 15) - (5, 19)] {
                        identifier ["namespace_func.cpp": (5, 19) - (5, 20)] x;
                        builtin_type_spec ["namespace_func.cpp": (5, 15) - (5, 15)] int;
                    }
                    function_parameter_decl ["namespace_func.cpp": (5, 22) - (5, 26)] {
                        identifier ["namespace_func.cpp": (5, 26) - (5, 27)] y;
                        builtin_type_spec ["namespace_func.cpp": (5, 22) - (5, 22)] int;
                    }
                }
            }
        }
        function_decl ["namespace_func.cpp": (8, 1) - (10, 1)] {
            identifier ["namespace_func.cpp": (8, 11) - (8, 14)] foo;
            namespace_scope_spec ["namespace_func.cpp": (8, 7) - (8, 9)] {
                identifier ["namespace_func.cpp": (8, 7) - (8, 9)] ns;
            }
            builtin_type_spec ["namespace_func.cpp": (8, 1) - (8, 1)] float;
            function_parameter_decl ["namespace_func.cpp": (8, 15) - (8, 19)] {
                identifier ["namespace_func.cpp": (8, 19) - (8, 20)] x;
                builtin_type_spec ["namespace_func.cpp": (8, 15) - (8, 15)] int;
            }
            function_parameter_decl ["namespace_func.cpp": (8, 22) - (8, 26)] {
                identifier ["namespace_func.cpp": (8, 26) - (8, 27)] y;
                builtin_type_spec ["namespace_func.cpp": (8, 22) - (8, 22)] int;
            }
            opaque {
            }
        }
    }
}
