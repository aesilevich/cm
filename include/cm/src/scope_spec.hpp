// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file scope_spec.hpp
/// Contains definition of the scope_spec class.

#pragma once

#include "identifier.hpp"


namespace cm::src {


/// Represents scope specifier (namespace or record) in source code
class scope_spec: virtual public ast_node {
public:
    /// Returns true if this scope specifier has parent scope specifier
    bool has_scope() const { return scope_ ? true : false; }

    /// Returns pointer to parent scope specifier
    scope_spec * scope() { return scope_.get(); }

    /// Returns const pointer to parent scope specifier
    const scope_spec * scope() const { return scope_.get(); }

    /// Sets parent scope specifier
    void set_scope(std::unique_ptr<scope_spec> && s) { scope_ = std::move(s); }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) override {
        if (has_scope()) {
            return v.traverse(scope_.get());
        } else {
            return true;
        }
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const override {
        if (has_scope()) {
            return v.traverse(scope_.get());
        } else {
            return true;
        }
    }

private:
    std::unique_ptr<scope_spec> scope_;         ///< Pointer to parent scope specifier
};


}
