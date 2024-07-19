// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file type_attr.hpp
/// Contains definition of the type_attr class.

#pragma once

#include "ast_node_attrs.hpp"
#include "type_spec.hpp"


namespace cm::src {


/// Type attribute for AST node
template <typename Derived>
class type_attr {
public:
    /// Returns pointer to type specifier for value
    type_spec * type() { return type_.get(); }

    /// Returns pointer to type specifier for value
    const type_spec * type() const { return type_.get(); }

    /// Sets type specifier for value
    void set_type(std::unique_ptr<type_spec> && tp) { type_ = std::move(tp); }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        // traversing type
        return v.traverse(type());
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        // traversing type
        return v.traverse(type());
    }

private:
    std::unique_ptr<type_spec> type_;       ///< Value type specifier
};


}
