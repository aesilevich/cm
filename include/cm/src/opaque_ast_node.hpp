// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file opaque_ast_node.hpp
/// Contains definition of the opaque_ast_node class.

#pragma once

#include "ast_node.hpp"
#include "ast_node_impl.hpp"


namespace cm::src {


/// Represents "opaque" block of source code with unknown structure and list
/// of independed ast nodes with known structure
class opaque_ast_node: public ast_node_impl<"opaque"> {
public:
    /// Constructs empty opaque node
    explicit opaque_ast_node(ast_node * parent):
        ast_node_impl<"opaque">{parent} {}

    /// Returns range of const children AST nodes, filtering them by AST node type
    template <typename ASTNode = ast_node>
    auto children() const {
        static_assert(std::derived_from<ASTNode, ast_node>, "invalid AST node filter type");

        if constexpr (std::same_as<ASTNode, ast_node>) {
            auto transform_fn = [](auto && ptr) { return const_cast<const ASTNode*>(ptr.get()); };
            return children_ | std::ranges::views::transform(transform_fn);
        } else {
            auto transform_fn = [](auto && ptr) {
                return dynamic_cast<const ASTNode*>(ptr.get());
            };

            auto filter_fn = [](const ASTNode * ent) {
                return ent != nullptr;
            };

            return children_
                | std::ranges::views::transform(transform_fn)
                | std::ranges::views::filter(filter_fn);
        }
    }

    /// Returns range of children AST nodes, filtering them by AST node type if requested
    template <typename ASTNode = ast_node>
    auto children() {
        static_assert(std::derived_from<ASTNode, ast_node>, "invalid AST node filter type");

        if constexpr (std::same_as<ASTNode, ast_node>) {
            auto transform_fn = [](auto && ptr) { return ptr.get(); };
            return children_ | std::ranges::views::transform(transform_fn);
        } else {
            auto transform_fn = [](auto && ptr) {
                return dynamic_cast<ASTNode*>(ptr.get());
            };

            auto filter_fn = [](ASTNode * ent) {
                return ent != nullptr;
            };

            return children_
                | std::ranges::views::transform(transform_fn)
                | std::ranges::views::filter(filter_fn);
        }
    }

    /// Adds child AST node
    void add_child(std::unique_ptr<ast_node> && c) {
        children_.emplace_back(std::move(c));
    }

    /// Removes child AST node
    void remove_child(ast_node * node) {
        auto it = std::ranges::find_if(children_, [node](auto && uptr) {
            return uptr.get() == node; }
        );
        assert(it != std::ranges::end(children_) && "can't find declaration in decl context");
        children_.erase(it);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) override {
        return traverse_nodes(children(), v);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const override {
        return traverse_nodes(children(), v);
    }

private:
    /// List of children nodes
    std::list<std::unique_ptr<ast_node>> children_;
};


}
