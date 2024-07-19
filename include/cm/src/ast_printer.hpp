// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_printer
/// Contains definition of the ast_printer class.

#pragma once

#include "std_ast_visitor.hpp"


namespace cm::src {

class source_code_model;


/// AST printer
class ast_printer: public const_std_ast_visitor<ast_printer> {
public:
    /// Constructs AST printer with reference to output stream
    explicit ast_printer(std::ostream & str);

    /// Sets whether should print source positions
    void set_print_pos(bool val) { print_pos_ = val; }

    /// Sets whether full paths (not only file names) be printed
    void set_print_full_paths(bool val) { print_full_paths_ = val; }

    /// Prints source code model to output stream
    void print(const source_code_model & mdl_);

    /// Traverses through identifier AST node
    bool traverse_identifier(const identifier * node);

    /// Traverses through generic AST node
    bool traverse_ast_node(const ast_node * node);

    /// Traverses through source file AST node
    bool traverse_source_file(const source_file * node);

    /// Traverses through builtin type specifier
    bool traverse_builtin_type_spec(const builtin_type_spec * node);

    // Traverses through record type declaration
    // bool traverse_record_type_decl(const record_type_decl * decl);

    // bool visit_function_decl(const function_decl * node);
    // bool visit_method_decl(const method_decl * node);
    // bool visit_functional_decl(const functional_decl * node);

    // bool visit_function_parameter_decl(const function_parameter_decl * node);

    // bool visit_type_template_parameter_decl(const type_template_parameter_decl * node);
    // bool visit_value_template_parameter_decl(const value_template_parameter_decl * node);
    // bool visit_template_record_decl(const template_record_decl * node);
    // bool visit_record_decl(const record_decl * node);
    // bool visit_template_decl(const template_decl * node);
    // bool visit_type_decl(const type_decl * node);

    // bool visit_namespace_decl(const namespace_decl * node);
    // bool visit_source_file(const source_file * node);
    // bool visit_decl_context(const decl_context * node);
    // bool visit_declaration(const declaration * node);

    // bool visit_type_template_argument_spec(const type_template_argument_spec * node);
    // bool visit_value_template_argument_spec(const value_template_argument_spec * node);
    // bool visit_template_argument_spec(const template_argument_spec * node);

    // bool visit_namespace_scope_spec(const namespace_scope_spec * node);
    // bool visit_record_scope_spec(const record_scope_spec * node);
    // bool visit_template_record_scope_spec(const template_record_scope_spec * node);
    // bool visit_template_record_instantiation_scope_spec(
    //     const template_record_instantiation_scope_spec * node);
    // bool visit_template_scope_spec(const template_scope_spec * node);
    // bool visit_scope_spec(const scope_spec * node);

    // bool visit_template_record_instantiation_type_spec(
    //     const template_record_instantiation_type_spec * node);
    // bool visit_builtin_type_spec(const builtin_type_spec * node);
    // bool visit_pointer_type_spec(const pointer_type_spec * node);
    // bool visit_lvalue_reference_type_spec(const lvalue_reference_type_spec * node);
    // bool visit_rvalue_reference_type_spec(const rvalue_reference_type_spec * node);
    // bool visit_array_type_spec(const array_type_spec * node);
    // bool visit_type_spec_with_base(const type_spec_with_base * node);
    // bool visit_function_type_spec(const function_type_spec * node);
    // bool visit_context_type_spec(const context_type_spec * node);
    // bool visit_type_spec(const type_spec * node);

    // bool visit_opaque_ast_node(const opaque_ast_node * node);
    // bool visit_ast_node(const ast_node * node);

private:
    /// Prints indentation spaces for current depth
    void print_indent() const;

    /// Prints node header with indentation
    void print_node_header(const ast_node * node) const;

    /// Prints node children
    void print_node_children(const ast_node * node);

    /// Prints declaration name with scope to output stream
    void print_decl_full_name(const declaration * decl) const;

    /// Prints scope to output stream
    void print_scope(const scope_spec * scope) const;

    /// Prints type to output stream
    void print_type(const type_spec * type) const;

    /// Prints template name and arguments to output stream
    template <std::ranges::range Args>
    requires (std::convertible_to<std::ranges::range_value_t<Args>, const template_argument_spec*>)
    void print_template_name_and_args_r(const template_spec * templ, Args && args) const {
        str_ << templ->name()->string() << '<';
        for (auto arg : args) {
            if (auto type_arg = dynamic_cast<const type_template_argument_spec*>(arg)) {
                print_type(type_arg->type());
            } else if (auto value_arg = dynamic_cast<const value_template_argument_spec*>(arg)) {
                str_ << "#value#";
            } else {
                assert(false && "unknown template argument type");
            }
        }
        str_ << '>';
    }


    std::ostream & str_;                ///< Reference to output stream
    unsigned int indent_ = 0;           ///< Current printing indent
    bool print_pos_ = true;             ///< Should print source positions?
    bool print_full_paths_ = false;     ///< Should print full paths?
};


}
