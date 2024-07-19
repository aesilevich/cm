// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file identitifer.hpp
/// Contains definition of the identifier class.

#pragma once

#include "ast_node_impl_base.hpp"


namespace cm::src {


/// Represents identifier in source code. Stores range in source code.
class identifier: public ast_node_impl_base<"identifier"> {
public:
    /// Constucts identifier with specified string
    explicit identifier(ast_node * parent, std::string str = {}):
        ast_node_impl_base<"identifier">{parent}, str_{std::move(str)} {}

    /// Returns identitier string value
    std::string_view string() const { return str_; }

    /// Sets identifier string value
    template <typename Arg>
    requires (std::assignable_from<std::string &, Arg>)
    void set_string(Arg && arg) {
        str_ = std::forward<Arg>(arg);
    }

    /// Returns code model entity associated with this AST node.
    /// Always returns code model entity associated with parent node.
    const class entity * entity() const override {
        assert(parent() != nullptr && "null parent of identifier node");
        return parent()->entity();
    }

    /// Returns code model entity associated with this AST node.
    /// Always returns code model entity associated with parent node.
    class entity * entity() override {
        assert(parent() != nullptr && "null parent of identifier node");
        return parent()->entity();
    }

    /// Traverses all children nodes. Does nothing, always returns true.
    bool traverse_children(ast_visitor & v) override {
        return true;
    }

    /// Traverses all children nodes. Does nothing, always returns true.
    bool traverse_children(const_ast_visitor & v) const override {
        return true;
    }

private:
    std::string str_;       ///< Identitier string
};


}
