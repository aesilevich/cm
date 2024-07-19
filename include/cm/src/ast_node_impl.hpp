// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_node_impl.hpp
/// Contains definition of the ast_node_impl class.

#pragma once

#include "ast_node.hpp"
#include "ast_node_attrs.hpp"
#include "ast_node_impl_base.hpp"
#include "name_attr.hpp"
#include "source_file_range.hpp"


namespace cm::src {


/// Helper class for derining intermediate classes for AST nodes
template <
    std::derived_from<ast_node> Base = ast_node,
    template <typename Derived> typename ... Attrs
>
class intermediate_ast_node_impl:
        public Base,
        public ast_node_attrs<intermediate_ast_node_impl<Base, Attrs...>, Attrs...>
{
    /// Type of attributes base class
    using attrs_t = ast_node_attrs<intermediate_ast_node_impl<Base, Attrs...>, Attrs...>;

public:
    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(ast_visitor & v) override {
        // traversing base class children
        if (!Base::traverse_children(v)) {
            return false;
        }

        // traversing attributes
        return attrs_t::traverse_attrs(v);
    }

    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(const_ast_visitor & v) const override {
        // traversing base class children
        if (!Base::traverse_children(v)) {
            return false;
        }

        // traversing attributes
        return attrs_t::const_traverse_attrs(v);
    }
};


/// Helper class for defining concrete (final) classes for AST nodes
template <
    ast_node_class_name Name,
    std::derived_from<ast_node> Base = ast_node,
    typename Parent = ast_node,
    template <typename Derived> typename ... Attrs
>
//requires (ast_node_attr<Attrs> && ...)
class ast_node_impl: public ast_node_impl_base<Name, Base, Parent>,
                     public ast_node_attrs<ast_node_impl<Name, Base, Parent, Attrs...>, Attrs...>
{
    /// Type of attributes base class
    using attrs_t = ast_node_attrs<ast_node_impl<Name, Base, Parent, Attrs...>, Attrs...>;

public:
    /// Constructs AST node
    explicit ast_node_impl(Parent * parent):
        ast_node_impl_base<Name, Base, Parent>{parent} {}

    /// Destroys AST node
    ~ast_node_impl() override = default;

    /// Returns code model entity associated with this AST node
    const typename attrs_t::entity_t * entity() const override {
        if constexpr (attrs_t::has_entity_attr) {
            return attrs_t::entity();
        } else {
            return nullptr;
        }
    }

    /// Returns code model entity associated with this AST node
    typename attrs_t::entity_t * entity() override {
        if constexpr (attrs_t::has_entity_attr) {
            return attrs_t::entity();
        } else {
            return nullptr;
        }
    }

    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(ast_visitor & v) override {
        // traversing base class children
        if (!Base::traverse_children(v)) {
            return false;
        }

        // traversing attributes
        return attrs_t::traverse_attrs(v);
    }

    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(const_ast_visitor & v) const override {
        // traversing base class children
        if (!Base::traverse_children(v)) {
            return false;
        }

        // traversing attributes
        return attrs_t::const_traverse_attrs(v);
    }
};


}
