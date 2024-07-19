source_file "typedef_record.cpp" {
    decl_context {
        record ["typedef_record.cpp": (4, 9) - (7, 1)] {
            decl_context {
                field_decl ["typedef_record.cpp": (5, 5) - (5, 9)] {
                    identifier ["typedef_record.cpp": (5, 9) - (5, 10)] x;
                    builtin_type_spec ["typedef_record.cpp": (5, 5) - (5, 5)] int;
                }
                method_decl ["typedef_record.cpp": (6, 5) - (6, 14)] {
                    identifier ["typedef_record.cpp": (6, 10) - (6, 13)] foo;
                    builtin_type_spec ["typedef_record.cpp": (6, 5) - (6, 5)] void;
                }
            }
        }
        typedef ["typedef_record.cpp": (4, 1) - (7, 13)] {
            identifier ["typedef_record.cpp": (7, 3) - (7, 13)] my_typedef;
            record_decl_type_spec ["typedef_record.cpp": (4, 9) - (7, 1)] {
                record ["typedef_record.cpp": (4, 9) - (7, 1)] {
                    decl_context {
                        field_decl ["typedef_record.cpp": (5, 5) - (5, 9)] {
                            identifier ["typedef_record.cpp": (5, 9) - (5, 10)] x;
                            builtin_type_spec ["typedef_record.cpp": (5, 5) - (5, 5)] int;
                        }
                        method_decl ["typedef_record.cpp": (6, 5) - (6, 14)] {
                            identifier ["typedef_record.cpp": (6, 10) - (6, 13)] foo;
                            builtin_type_spec ["typedef_record.cpp": (6, 5) - (6, 5)] void;
                        }
                    }
                }
            }
        }
    }
}
