// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file std_ast_visitor.hpp
/// Contains definition of the std_ast_visitor class.

#pragma once

#include "ast.hpp"
#include <type_traits>


namespace cm::src::std_visitor_impl {

#define DEFINE_HAS_TRAVERSE_CONCEPT(type) \
    template <typename Visitor> \
    concept has_traverse_##type = requires(Visitor & v, type * decl) { \
        { v.traverse_##type(decl) } -> std::convertible_to<bool>; \
    };

#define DEFINE_HAS_VISIT_CONCEPT(type) \
    template <typename Visitor> \
    concept has_visit_##type = requires(Visitor & v, type * decl) { \
        { v.visit_##type(decl) } -> std::convertible_to<bool>; \
    };

#define DEFINE_HAS_CONCEPTS(type) \
    DEFINE_HAS_TRAVERSE_CONCEPT(type) \
    DEFINE_HAS_VISIT_CONCEPT(type)

DEFINE_HAS_CONCEPTS(ast_node)
DEFINE_HAS_CONCEPTS(decl_context)
DEFINE_HAS_CONCEPTS(declaration)
DEFINE_HAS_CONCEPTS(function_decl)
DEFINE_HAS_CONCEPTS(function_parameter_decl)
DEFINE_HAS_CONCEPTS(functional_decl)
DEFINE_HAS_CONCEPTS(identifier)
DEFINE_HAS_CONCEPTS(method_decl)
DEFINE_HAS_CONCEPTS(namespace_decl)
DEFINE_HAS_CONCEPTS(namespace_scope_spec)
DEFINE_HAS_CONCEPTS(opaque_ast_node)
DEFINE_HAS_CONCEPTS(record_decl)
DEFINE_HAS_CONCEPTS(record_scope_spec)
DEFINE_HAS_CONCEPTS(scope_spec)
DEFINE_HAS_CONCEPTS(source_file)
DEFINE_HAS_CONCEPTS(template_argument_spec)
DEFINE_HAS_CONCEPTS(type_template_argument_spec)
DEFINE_HAS_CONCEPTS(value_template_argument_spec)
DEFINE_HAS_CONCEPTS(type_template_parameter_decl)
DEFINE_HAS_CONCEPTS(value_template_parameter_decl)
DEFINE_HAS_CONCEPTS(template_decl)
DEFINE_HAS_CONCEPTS(template_record_instantiation_type_spec)
DEFINE_HAS_CONCEPTS(template_scope_spec)
DEFINE_HAS_CONCEPTS(template_record_scope_spec)
DEFINE_HAS_CONCEPTS(template_record_instantiation_scope_spec)
DEFINE_HAS_CONCEPTS(type_decl)
DEFINE_HAS_CONCEPTS(type_spec)
DEFINE_HAS_CONCEPTS(builtin_type_spec)
DEFINE_HAS_CONCEPTS(type_spec_with_base)
DEFINE_HAS_CONCEPTS(pointer_type_spec)
DEFINE_HAS_CONCEPTS(lvalue_reference_type_spec)
DEFINE_HAS_CONCEPTS(rvalue_reference_type_spec)
DEFINE_HAS_CONCEPTS(array_type_spec)
DEFINE_HAS_CONCEPTS(function_type_spec)
DEFINE_HAS_CONCEPTS(context_type_spec)

#undef DEFINE_HAS_TRAVERSE_CONCEPT
#undef DEFINE_HAS_VISIT_CONCEPT
#undef DEFINE_HAS_CONCEPTS


/// Standard AST visitor that dispatches visiting based on known standard AST nodes types
template <bool Const, typename Derived>
class basic_std_ast_visitor:
    public std::conditional_t<Const, const_ast_visitor, ast_visitor>
{
    template <typename Node>
    using add_const_t = std::conditional_t<Const, const Node, Node>;

public:
    /// Traverses through abstract AST node. Performs static dispatch to derived class.
    bool traverse(add_const_t<ast_node> * node) override {

#define TRAVERSE_NODE(type) \
        if constexpr(has_traverse_##type<Derived>) { \
            if (auto cnode = dynamic_cast<add_const_t<type> *>(node)) { \
                return static_cast<Derived*>(this)->traverse_##type(cnode); \
            } \
        }

        // functional declarations
        TRAVERSE_NODE(function_decl)
        TRAVERSE_NODE(method_decl)
        TRAVERSE_NODE(functional_decl)
        TRAVERSE_NODE(function_parameter_decl)

        // types
        TRAVERSE_NODE(type_template_parameter_decl)
        TRAVERSE_NODE(value_template_parameter_decl)
        TRAVERSE_NODE(record_decl)
        TRAVERSE_NODE(template_decl)
        TRAVERSE_NODE(type_decl)

        TRAVERSE_NODE(namespace_decl)
        TRAVERSE_NODE(source_file)
        TRAVERSE_NODE(decl_context)
        TRAVERSE_NODE(declaration)

        // specifiers
        TRAVERSE_NODE(namespace_scope_spec)
        TRAVERSE_NODE(record_scope_spec)
        TRAVERSE_NODE(type_template_argument_spec)
        TRAVERSE_NODE(value_template_argument_spec)
        TRAVERSE_NODE(template_argument_spec)
        TRAVERSE_NODE(template_record_instantiation_type_spec)
        TRAVERSE_NODE(template_record_scope_spec)
        TRAVERSE_NODE(template_record_instantiation_scope_spec)
        TRAVERSE_NODE(template_scope_spec)
        TRAVERSE_NODE(scope_spec)

        TRAVERSE_NODE(builtin_type_spec)
        TRAVERSE_NODE(pointer_type_spec)
        TRAVERSE_NODE(lvalue_reference_type_spec)
        TRAVERSE_NODE(rvalue_reference_type_spec)
        TRAVERSE_NODE(array_type_spec)
        TRAVERSE_NODE(type_spec_with_base)
        TRAVERSE_NODE(function_type_spec)
        TRAVERSE_NODE(context_type_spec)
        TRAVERSE_NODE(type_spec)

        TRAVERSE_NODE(identifier)

        TRAVERSE_NODE(opaque_ast_node)
        TRAVERSE_NODE(ast_node)

        // no traverse function was dispatched to derived class, traversing through node here

        // visiting this node
        if (!do_visit_node(node)) {
            return false;
        }

        // traversing children
        return node->traverse_children(*this);
    }

#define DEFINE_VISIT_FUNC(type) \
    bool visit_##type(add_const_t<type> * node) { \
        node_visited_ = false; \
        return true; \
    }

    DEFINE_VISIT_FUNC(ast_node)
    DEFINE_VISIT_FUNC(decl_context)
    DEFINE_VISIT_FUNC(declaration)
    DEFINE_VISIT_FUNC(function_decl)
    DEFINE_VISIT_FUNC(function_parameter_decl)
    DEFINE_VISIT_FUNC(functional_decl)
    DEFINE_VISIT_FUNC(method_decl)
    DEFINE_VISIT_FUNC(namespace_decl)
    DEFINE_VISIT_FUNC(namespace_scope_spec)
    DEFINE_VISIT_FUNC(identifier)
    DEFINE_VISIT_FUNC(opaque_ast_node)
    DEFINE_VISIT_FUNC(record_decl)
    DEFINE_VISIT_FUNC(record_scope_spec)
    DEFINE_VISIT_FUNC(scope_spec)
    DEFINE_VISIT_FUNC(source_file)
    DEFINE_VISIT_FUNC(template_argument_spec)
    DEFINE_VISIT_FUNC(type_template_argument_spec)
    DEFINE_VISIT_FUNC(value_template_argument_spec)
    DEFINE_VISIT_FUNC(type_template_parameter_decl)
    DEFINE_VISIT_FUNC(value_template_parameter_decl)
    DEFINE_VISIT_FUNC(template_decl)
    DEFINE_VISIT_FUNC(template_record_instantiation_type_spec)
    DEFINE_VISIT_FUNC(template_scope_spec)
    DEFINE_VISIT_FUNC(template_record_scope_spec)
    DEFINE_VISIT_FUNC(template_record_instantiation_scope_spec)
    DEFINE_VISIT_FUNC(type_decl)
    DEFINE_VISIT_FUNC(type_spec)
    DEFINE_VISIT_FUNC(builtin_type_spec)
    DEFINE_VISIT_FUNC(type_spec_with_base)
    DEFINE_VISIT_FUNC(pointer_type_spec)
    DEFINE_VISIT_FUNC(lvalue_reference_type_spec)
    DEFINE_VISIT_FUNC(rvalue_reference_type_spec)
    DEFINE_VISIT_FUNC(array_type_spec)
    DEFINE_VISIT_FUNC(function_type_spec)
    DEFINE_VISIT_FUNC(context_type_spec)

#undef DEFINE_VISIT_FUNC


private:
    /// Visits single node. Returns true if visiting should continue.
    bool do_visit_node(add_const_t<ast_node> * node) {
        auto & dthis = static_cast<Derived&>(*this);

#define VISIT_NODE(type) \
        if constexpr(has_visit_##type<Derived>) { \
            if (auto cnode = dynamic_cast<add_const_t<type> *>(node)) { \
                return static_cast<Derived*>(this)->visit_##type(cnode); \
            } \
        }

        // calling visit functions accoring to oreder in hierarchy

        // functional declarations
        VISIT_NODE(function_decl)
        VISIT_NODE(method_decl)
        VISIT_NODE(functional_decl)

        VISIT_NODE(function_parameter_decl)

        // types
        VISIT_NODE(type_template_parameter_decl)
        VISIT_NODE(value_template_parameter_decl)
        VISIT_NODE(record_decl)
        VISIT_NODE(template_decl)
        VISIT_NODE(type_decl)

        VISIT_NODE(namespace_decl)
        VISIT_NODE(source_file)
        VISIT_NODE(decl_context)
        VISIT_NODE(declaration)

        // specifiers
        VISIT_NODE(namespace_scope_spec)
        VISIT_NODE(record_scope_spec)
        VISIT_NODE(type_template_argument_spec)
        VISIT_NODE(value_template_argument_spec)
        VISIT_NODE(template_argument_spec)
        VISIT_NODE(template_record_instantiation_type_spec)
        VISIT_NODE(template_record_scope_spec)
        VISIT_NODE(template_record_instantiation_scope_spec)
        VISIT_NODE(template_scope_spec)
        VISIT_NODE(scope_spec)

        VISIT_NODE(builtin_type_spec)
        VISIT_NODE(pointer_type_spec)
        VISIT_NODE(lvalue_reference_type_spec)
        VISIT_NODE(rvalue_reference_type_spec)
        VISIT_NODE(array_type_spec)
        VISIT_NODE(type_spec_with_base)
        VISIT_NODE(function_type_spec)
        VISIT_NODE(context_type_spec)
        VISIT_NODE(type_spec)

        VISIT_NODE(identifier)

        VISIT_NODE(opaque_ast_node)
        VISIT_NODE(ast_node)

#undef VISIT_NODE

        return true;
    }


    /// Was current node visited in derived class?
    bool node_visited_ = false;
};


template <typename Derived>
using std_ast_visitor = basic_std_ast_visitor<false, Derived>;

template <typename Derived>
using const_std_ast_visitor = basic_std_ast_visitor<true, Derived>;


}


namespace cm::src {

template <typename Derived>
using std_ast_visitor = std_visitor_impl::std_ast_visitor<Derived>;

template <typename Derived>
using const_std_ast_visitor = std_visitor_impl::const_std_ast_visitor<Derived>;

}
