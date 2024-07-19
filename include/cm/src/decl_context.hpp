// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file decl_context.hpp
/// Contains definition of the decl_context class.

#pragma once

#include "ast_node_impl.hpp"
#include "declaration.hpp"
#include <list>


namespace cm::src {


/// List of declarations AST node attribute
template <typename Derived>
class declarations_attr {
public:
    /// Returns range of const declarations in decl context, filtering them by declaration type
    template <typename Declaration = declaration>
    auto decls() const {
        static_assert(std::derived_from<Declaration, declaration>, "invalid declaration filter type");

        if constexpr (std::same_as<Declaration, declaration>) {
            auto transform_fn = [](auto && ptr) { return const_cast<const declaration*>(ptr.get()); };
            return decls_ | std::ranges::views::transform(transform_fn);
        } else {
            auto transform_fn = [](auto && ptr) {
                return dynamic_cast<const Declaration*>(ptr.get());
            };

            auto filter_fn = [](const Declaration * ent) {
                return ent != nullptr;
            };

            return decls_
                | std::ranges::views::transform(transform_fn)
                | std::ranges::views::filter(filter_fn);
        }
    }

    /// Returns range of declarations in context, filtering them by entity type if requested
    template <typename Declaration = declaration>
    auto decls() {
        static_assert(std::derived_from<Declaration, declaration>, "invalid declaration filter type");

        if constexpr (std::same_as<Declaration, declaration>) {
            auto transform_fn = [](auto && ptr) { return ptr.get(); };
            return decls_ | std::ranges::views::transform(transform_fn);
        } else {
            auto transform_fn = [](auto && ptr) {
                return dynamic_cast<Declaration*>(ptr.get());
            };

            auto filter_fn = [](Declaration * ent) {
                return ent != nullptr;
            };

            return decls_
                | std::ranges::views::transform(transform_fn)
                | std::ranges::views::filter(filter_fn);
        }
    }

    /// Adds declaration to context. Returns pointer to declaration
    template <std::derived_from<declaration> Declaration>
    Declaration * add_decl(std::unique_ptr<Declaration> && decl) {
        auto decl_ptr = decl.get();
        decls_.push_back(std::move(decl));
        return decl_ptr;
    }

    /// Creates declaration of specified type. Returns pointer to declaration
    template <std::derived_from<declaration> Declaration, typename ... Args>
    requires (std::constructible_from<Declaration, Derived *, Args...>)
    Declaration * create_decl(Args && ... args) {
        auto decl = std::make_unique<Declaration>(static_cast<Derived*>(this),
                                                  std::forward<Args>(args)...);
        return add_decl(std::move(decl));
    }

    /// Removes declaration from context
    void remove_decl(declaration * decl) {
        auto it = std::ranges::find_if(decls_, [decl](auto && uptr) { return uptr.get() == decl; });
        assert(it != std::ranges::end(decls_) && "can't find declaration in decl context");
        decls_.erase(it);
    }

    /// Traverses all declarations. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        return ast_node::traverse_nodes(decls(), v);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        return ast_node::traverse_nodes(decls(), v);
    }

private:
    std::list<std::unique_ptr<declaration>> decls_;     ///< List of declaration in context
};


using decl_context = ast_node_impl <
    "decl_context",
    ast_node,
    ast_node,
    declarations_attr
>;


}
