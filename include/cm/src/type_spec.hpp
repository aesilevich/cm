// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file type_spec.hpp
/// Contains definition of type_spec type.

#pragma once

#include "entity_attr.hpp"
#include "scope_spec.hpp"
#include <list>


namespace cm::src {


/// Represents type specifier in source code. Each type specifier references some
/// type in code model.
class type_spec: virtual public ast_node {
public:
};


/// Builtin type specifier
using builtin_type_spec = ast_node_impl <
    "builtin_type_spec",
    type_spec,
    ast_node,
    entity_attr<builtin_type>::type
>;


/// Type specifier with base type (pointer like, array, etc)
class type_spec_with_base: public type_spec {
public:
    /// Returns pointer to base type specifier
    const type_spec * base() const { return base_.get(); }

    /// Sets base type specifier
    void set_base(std::unique_ptr<type_spec> && b) {
        base_ = std::move(b);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) override {
        return v.traverse(base_.get());
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const override {
        return v.traverse(base_.get());
    }

private:
    std::unique_ptr<type_spec> base_;   ///< Base type specifier
};


/// Represents pointer type specifier in source code
using pointer_type_spec = ast_node_impl <
    "pointer_type_spec",
    type_spec_with_base,
    ast_node,
    entity_attr<pointer_type>::type
>;

/// Represents lvalue reference type specifier in source code
using lvalue_reference_type_spec = ast_node_impl <
    "lvalue_reference_type_spec",
    type_spec_with_base,
    ast_node,
    entity_attr<lvalue_reference_type>::type
>;

/// Represents rvalue reference type specifier in source code
using rvalue_reference_type_spec = ast_node_impl <
    "rvalue_reference_type_spec",
    type_spec_with_base,
    ast_node,
    entity_attr<rvalue_reference_type>::type
>;

/// Represents array type specifier
using array_type_spec = ast_node_impl <
    "array_type_spec",
    type_spec_with_base,
    ast_node,
    entity_attr<array_type>::type
>;

/// Represents parens () in type specifier
using parens_type_spec = ast_node_impl <
    "parens_type_spec",
    type_spec_with_base,
    ast_node
>;


/// Represents specification of a type located in some context
using context_type_spec = intermediate_ast_node_impl <
    type_spec,
    scope_attr
>;


}
