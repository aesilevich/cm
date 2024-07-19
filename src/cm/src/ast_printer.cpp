// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_printer.cpp
/// Contains implementation of the ast_printer class

#include "pch.hpp"
#include "cm/src/ast_printer.hpp"


namespace cm::src {


ast_printer::ast_printer(std::ostream & str):
str_{str} {
}


void ast_printer::print(const source_code_model & mdl_) {
    for (auto && src : mdl_.sources()) {
        traverse(src);
    }
}


bool ast_printer::traverse_identifier(const identifier * node) {
    print_node_header(node);
    str_ << ' ' << node->string() << ";\n";
    return true;
}


bool ast_printer::traverse_ast_node(const ast_node * node) {
    print_node_header(node);
    print_node_children(node);
    return true;
}


bool ast_printer::traverse_source_file(const source_file * node) {
    print_node_header(node);

    str_ << ' ';

    if (print_full_paths_) {
        str_ << node->cm_src()->path();
    } else {
        str_ << node->cm_src()->path().filename();
    }

    print_node_children(node);
    return true;
}


bool ast_printer::traverse_builtin_type_spec(const builtin_type_spec * node) {
    print_node_header(node);
    str_ << ' ' << node->entity()->name() << ";\n";
    return true;
}


// bool ast_printer::traverse_record_type_decl(const record_type_decl * decl) {
//     // printing record header
//     print_indent();
//     str_ << "record ";
//     if (decl->has_name()) {
//         str_ << decl->name()->string() << ' ';
//     }
//     str_ << "{\n";

//     // printing contents
//     decl->traverse_children(*this);

//     // printing record footer
//     print_indent();
//     str_ << "}\n";

//     return true;
// }


// bool ast_printer::visit_function_decl(const function_decl * node) {
//     print_indent();
//     //str << "func " << node->entity()
//     return true;
// }


// bool ast_printer::visit_method_decl(const method_decl * node) {
//     return true;
// }


// bool ast_printer::visit_functional_decl(const functional_decl * node) {
//     return true;
// }


// bool ast_printer::visit_function_parameter_decl(const function_parameter_decl * node) {
//     return true;
// }


// bool ast_printer::visit_type_template_parameter_decl(const type_template_parameter_decl * node) {
//     return true;
// }


// bool ast_printer::visit_value_template_parameter_decl(const value_template_parameter_decl * node) {
//     return true;
// }


// bool ast_printer::visit_template_record_decl(const template_record_decl * node) {
//     return true;
// }


// bool ast_printer::visit_record_decl(const record_decl * node) {
//     return true;
// }


// bool ast_printer::visit_template_decl(const template_decl * node) {
//     return true;
// }


// bool ast_printer::visit_type_decl(const type_decl * node) {
//     return true;
// }


// bool ast_printer::visit_namespace_decl(const namespace_decl * node) {
//     // printing header
//     print_indent();
//     str_ << "namespace ";
//     if (node->has_name()) {
//         str_ << node->name()->string() << ' ';
//     }
//     str_ << "{\n";

//     // printing footer
//     str_ << "}\n";

//     return true;
// }


// bool ast_printer::visit_source_file(const source_file * node) {
//     return true;
// }


// bool ast_printer::visit_decl_context(const decl_context * node) {
//     return true;
// }


// bool ast_printer::visit_declaration(const declaration * node) {
//     return true;
// }


// bool ast_printer::visit_namespace_scope_spec(const namespace_scope_spec * node) {
//     return true;
// }


// bool ast_printer::visit_record_scope_spec(const record_scope_spec * node) {
//     return true;
// }


// bool ast_printer::visit_type_template_argument_spec(const type_template_argument_spec * node) {
//     return true;
// }


// bool ast_printer::visit_value_template_argument_spec(const value_template_argument_spec * node) {
//     return true;
// }


// bool ast_printer::visit_template_argument_spec(const template_argument_spec * node) {
//     return true;
// }


// bool ast_printer::visit_template_record_instantiation_type_spec(
//     const template_record_instantiation_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_template_record_scope_spec(const template_record_scope_spec * node) {
//     return true;
// }


// bool ast_printer::visit_template_record_instantiation_scope_spec(
//     const template_record_instantiation_scope_spec * node) {
//     return true;
// }


// bool ast_printer::visit_template_scope_spec(const template_scope_spec * node) {
//     return true;
// }


// bool ast_printer::visit_scope_spec(const scope_spec * node) {
//     return true;
// }


// bool ast_printer::visit_builtin_type_spec(const builtin_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_pointer_type_spec(const pointer_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_lvalue_reference_type_spec(const lvalue_reference_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_rvalue_reference_type_spec(const rvalue_reference_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_array_type_spec(const array_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_type_spec_with_base(const type_spec_with_base * node) {
//     return true;
// }


// bool ast_printer::visit_function_type_spec(const function_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_context_type_spec(const context_type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_type_spec(const type_spec * node) {
//     return true;
// }


// bool ast_printer::visit_opaque_ast_node(const opaque_ast_node * node) {
//     return true;
// }


// bool ast_printer::visit_ast_node(const ast_node * node) {
//     return true;
// }


void ast_printer::print_indent() const {
    for (unsigned int i = 0; i < indent_; ++i) {
        str_ << "    ";
    }
}


void ast_printer::print_node_header(const ast_node * node) const {
    // printing node name
    print_indent();
    str_ << node->class_name();

    // printing source positions
    if (print_pos_) {
        auto range = node->source_range();
        if (range.is_valid()) {
            range.print(str_, print_full_paths_);
        }
    }

    // if (auto ent = node->entity()) {
    //     str_ << " [entity = " << ent << ", ";
    //     ent->print_desc(str_);
    //     str_ << "]";
    // }
}


void ast_printer::print_node_children(const ast_node * node) {
    str_ << " {\n";

    ++indent_;
    node->traverse_children(*this);
    --indent_;

    print_indent();
    str_ << "}\n";
}


void ast_printer::print_decl_full_name(const declaration * decl) const {

}


void ast_printer::print_scope(const scope_spec * scope) const {
    // printing parnet scope
    if (scope->has_scope()) {
        print_scope(scope->scope());
        str_ << "::";
    }

    if (auto ns = dynamic_cast<const namespace_scope_spec*>(scope)) {
        str_ << ns->name()->string();
    } else if (auto rec = dynamic_cast<const record_scope_spec*>(scope)) {
        str_ << rec->name()->string();
    } else if (auto templ = dynamic_cast<const template_record_scope_spec*>(scope)) {
        print_template_name_and_args_r(templ->templ(), templ->arguments());
    } else if (auto templ = dynamic_cast<const template_record_instantiation_scope_spec*>(scope)) {
        print_template_name_and_args_r(templ->templ(), templ->arguments());
    } else {
        assert(false && "unknown scope specifier type");
    }
}


void ast_printer::print_type(const type_spec * type) const {
    if (auto bt_type = dynamic_cast<const builtin_type_spec*>(type)) {
        str_ << bt_type->entity()->name();
    } else if (auto ptr_type = dynamic_cast<const pointer_type_spec*>(type)) {
        print_type(ptr_type->base());
        str_ << " *";
    } else if (auto lref_type = dynamic_cast<const lvalue_reference_type_spec*>(type)) {
        print_type(ptr_type->base());
        str_ << " &";
    } else if (auto rref_type = dynamic_cast<const rvalue_reference_type_spec*>(type)) {
        print_type(ptr_type->base());
        str_ << " &&";
    } else if (auto arr_type = dynamic_cast<const array_type_spec*>(type)) {
        print_type(arr_type->base());
        str_ << "[]";
    } else if (auto f_type = dynamic_cast<const function_type_spec*>(type)) {
        // str_ << '(';
        // bool first = true;
        // for (auto && par : f_type->params()) {
        //     if (!first) {
        //         str_ << ", ";
        //     } else {
        //         first = false;
        //     }

        //     print_type(par);
        // }
        // str_ << ')';
        // if (f_type->ret_type()) {
        //     str_ << " -> ";
        //     print_type(f_type->ret_type());
        // }
    } else if (auto t_type = dynamic_cast<const template_record_instantiation_type_spec*>(type)) {
        print_template_name_and_args_r(t_type->templ(), t_type->arguments());
    } else if (auto ctx_type = dynamic_cast<const context_type_spec*>(type)) {
    } else {
        assert(false && "unknown type specifier type");
    }
}


}
