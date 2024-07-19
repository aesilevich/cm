// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_node_attrs.hpp
/// Contains definitions of types related to AST node attributes.

#pragma once

#include "ast_visitor.hpp"
#include "entity_attr.hpp"


namespace cm::src {


/// AST node attribute
template <typename Attr>
concept ast_node_attr = requires(Attr & attr, const Attr & cattr) {
    { attr.traverse_children(std::declval<ast_visitor &>()) } -> std::convertible_to<bool>;
    { cattr.traverse_children(std::declval<const_ast_visitor &>()) } -> std::convertible_to<bool>;
};


namespace impl {
    /// Helper class for detecting code model entity type from attribute
    template <typename Attr>
    struct entity_attr_type {
        using type = void;
    };

    template <typename Derived, typename Entity>
    struct entity_attr_type<entity_attr_impl<Entity, Derived>> {
        using type = Entity;
    };

    template <typename Attr>
    using entity_attr_type_t = typename entity_attr_type<Attr>::type;

    /// Helper class for searching entity attribute in pack of attributes
    template <typename ... Attrs>
    struct entity_attrs_type {
        using type = void;
    };

    template <typename FirstAttr, typename ... Attrs>
    struct entity_attrs_type<FirstAttr, Attrs...> {
        using type = std::conditional_t <
            std::is_same_v<entity_attr_type_t<FirstAttr>, void>,
            typename entity_attrs_type<Attrs...>::type,
            entity_attr_type_t<FirstAttr>
        >;
    };

    template <typename ... Attrs>
    using entity_attrs_type_t = typename entity_attrs_type<Attrs...>::type;

    static_assert(std::same_as<entity_attrs_type_t<entity_attr<type_t>::type<void>>, type_t>);
}


/// Helper class for defining attributes AST node classes
template <
    typename Derived,
    template <typename AttrDerived> typename ... Attrs
>
requires (ast_node_attr<Attrs<Derived>> && ...)
class ast_node_attrs: public Attrs<Derived>...
{
    /// This type
    using this_t = ast_node_attrs<Derived, Attrs...>;

    /// Helper class for calling visit_children methods for all attributes
    template <typename ... Attrs2>
    struct attrs_tag {};

    using all_attrs_tag = attrs_tag<Attrs<Derived>...>;

    /// Type of entity contained in attributes
    using real_entity_t = impl::entity_attrs_type_t<Attrs<Derived>...>;

protected:
    /// Does attribute list contain entity?
    static constexpr bool has_entity_attr = !std::is_same_v<real_entity_t, void>;

    /// Type of entity for derived class
    using entity_t = std::conditional_t <
        has_entity_attr,
        real_entity_t,
        entity
    >;

    /// Traverses all attributes nodes. Returns true if traversing should continue.
    bool traverse_attrs(ast_visitor & v) {
        // traversing attributes
        return traverse_attrs_impl(v, all_attrs_tag{});
    }

    /// Traverses all attributes nodes. Returns true if traversing should continue.
    bool const_traverse_attrs(const_ast_visitor & v) const {
        // traversing attributes
        return const_traverse_attrs_impl(v, all_attrs_tag{});
    }

private:
    /// Traverses attributes
    template <typename First, typename ... Tail>
    bool traverse_attrs_impl(ast_visitor & v, attrs_tag<First, Tail...>) {
        if (!First::traverse_children(v)) {
            return false;
        }

        return traverse_attrs_impl(v, attrs_tag<Tail...>{});
    }

    /// Traverses attributes
    bool traverse_attrs_impl(ast_visitor & v, attrs_tag<>) {
        return true;
    }

    /// Traverses attributes
    template <typename First, typename ... Tail>
    bool const_traverse_attrs_impl(const_ast_visitor & v, attrs_tag<First, Tail...>) const {
        if (!First::traverse_children(v)) {
            return false;
        }

        return const_traverse_attrs_impl(v, attrs_tag<Tail...>{});
    }

    /// Traferses attributes
    bool const_traverse_attrs_impl(const_ast_visitor & v, attrs_tag<>) const {
        return true;
    }
};


}
