// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file scope_attr.hpp
/// Contains definition of the scope_attr class.

#pragma once


namespace cm::src {


/// Represents declaration that may have scope specifier
template <typename Derived>
class scope_attr {
public:
    /// Returns true if declaration has parent scope specifier
    bool has_scope() const { return scope_ ? true : false; }

    /// Returns pointer to parent scope specifier
    scope_spec * scope() { return scope_.get(); }

    /// Returns const pointer to parent scope specifier
    const scope_spec * scope() const { return scope_.get(); }

    /// Sets parent scope specifier
    void set_scope(std::unique_ptr<scope_spec> && s) { scope_ = std::move(s); }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        if (has_scope()) {
            return v.traverse(scope_.get());
        } else {
            return true;
        }
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        if (has_scope()) {
            return v.traverse(scope_.get());
        } else {
            return true;
        }
    }

private:
    std::unique_ptr<scope_spec> scope_;     ///< Declaration scope specifier
};


}
