// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file decl_context_attr.hpp
/// Contains definition of the decl_context_attr class.

#pragma once

#include "ast_node_attrs.hpp"
#include "declaration.hpp"
#include <list>


namespace cm::src {


/// Declaration context attribute for ast node
template <typename Derived>
class decl_context_attr {
public:
    /// Returns true if node has declaration context
    bool has_decl_ctx() const {
        return decl_ctx_ ? true : false;
    }

    /// Returns pointer to nested delcaration context for this node
    decl_context * decl_ctx() {
        return decl_ctx_.get();
    }

    /// Returns pointer to nested delcaration context for this node
    const decl_context * decl_ctx() const {
        return decl_ctx_.get();
    }

    /// Sets nested declaration context for this node
    void set_decl_context(std::unique_ptr<decl_context> && ctx) {
        decl_ctx_ = std::move(ctx);
    }

    /// Creates nested declaration context for AST node
    void create_decl_ctx() {
        assert(!has_decl_ctx() && "nested declaration context already exists");
        set_decl_context(std::make_unique<decl_context>(static_cast<Derived*>(this)));
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        if (has_decl_ctx()) {
            return v.traverse(decl_ctx());
        } else {
            return true;
        }
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        if (has_decl_ctx()) {
            return v.traverse(decl_ctx());
        } else {
            return true;
        }
    }

private:
    std::unique_ptr<decl_context> decl_ctx_;        ///< Declaration context
};


}
