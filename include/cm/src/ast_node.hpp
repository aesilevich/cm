// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_node.hpp
/// Contains definition of the ast_node and inner_ast_node classes.

#pragma once

#include "ast_visitor.hpp"
#include "source_file_range.hpp"
#include "../cm.hpp"
#include <list>
#include <memory>
#include <ranges>


namespace cm::src {


/// Represents node in source code AST tree.
class ast_node: public entity_use {
public:
    /// Default virtual destructor
    virtual ~ast_node() = default;

    /// Returns AST node class name
    virtual std::string class_name() const = 0;

    /// Returns pointer to parent node
    virtual ast_node * parent() = 0;

    /// Returns pointer to parent node
    virtual const ast_node * parent() const = 0;

    /// Returns range in source code for AST node
    virtual source_file_range source_range() const = 0;

    /// Sets range in source code for AST node
    virtual void set_source_range(const source_file_range & r) = 0;

    /// Returns code model entity associated with this AST node
    virtual const class entity * entity() const = 0;

    /// Returns code model entity associated with this AST node
    virtual class entity * entity() = 0;

    /// Sets start position of AST node source range
    void set_source_range_start(const source_position & pos) {
        auto r = source_range();
        r.range().set_start(pos);
        set_source_range(r);
    }

    /// Sets end position of AST node source range
    void set_source_range_end(const source_position & pos) {
        auto r = source_range();
        r.range().set_end(pos);
        set_source_range(r);
    }

    /// Adjusts AST node source range to include specified source range
    void adjust_source_range(const src::source_range & r) {
        if (r.start() < source_range().range().start()) {
            set_source_range_start(r.start());
        }

        if (r.end() > source_range().range().end()) {
            set_source_range_end(r.end());
        }
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    virtual bool traverse_children(ast_visitor & v) { return true; }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    virtual bool traverse_children(const_ast_visitor & v) const { return true; }

    /// Traverses all nodes in specified range
    template <std::ranges::range Range>
    requires (std::convertible_to<std::ranges::range_value_t<Range>, ast_node *>)
    static bool traverse_nodes(Range && nodes, ast_visitor & v) {
        for (auto && node : nodes) {
            if (!v.traverse(node)) {
                return false;
            }
        }

        return true;
    }

    /// Traverses all nodes in specified range
    template <std::ranges::range Range>
    requires (std::convertible_to<std::ranges::range_value_t<Range>, const ast_node *>)
    static bool traverse_nodes(Range && nodes, const_ast_visitor & v) {
        for (auto && node : nodes) {
            if (!v.traverse(node)) {
                return false;
            }
        }

        return true;
    }
};


}
