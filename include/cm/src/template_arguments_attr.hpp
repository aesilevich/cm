// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_arguments_attr.hpp
/// Contains definition of the template_arguments_attr class.

#pragma once

#include "template_substitution.hpp"


namespace cm::src {


/// AST node attribute for template arguments list
template <typename Derived>
class template_arguments_attr {
public:
    /// Returns reference to list of template arguments
    auto & arguments_list() { return arguments_; }

    /// Returns range of template arguments
    auto arguments() {
        auto fn = [](auto && uptr) { return uptr.get(); };
        return arguments_ | std::ranges::views::transform(fn);
    }

    /// Returns const range of template arguments
    auto arguments() const {
        auto fn = [](auto && uptr) {
            return const_cast<const template_argument_spec*>(uptr.get());
        };

        return arguments_ | std::ranges::views::transform(fn);
    }

    /// Adds template argument specifier into list of arguments
    void add_arg(std::unique_ptr<template_argument_spec> && arg) {
        arguments_list().push_back(std::move(arg));
    }

    /// Returns template argument previous to specified. Returns null if specified argument
    /// is the first in the list of arguments.
    template_argument_spec * prev_arg(template_argument_spec * arg) {
        return prev_arg_impl(this);
    }

    /// Returns template argument previous to specified. Returns null if specified argument
    /// is the first in the list of arguments.
    const template_argument_spec * prev_arg(const template_argument_spec * arg) const {
        return prev_arg_impl(this);
    }

    /// Returns template argument next to specified. Returns null if specified argument
    /// is the last in the list of arguments.
    template_argument_spec * next_arg(template_argument_spec * arg) {
        return next_arg_impl(this);
    }

    /// Returns template argument next to specified. Returns null if specified argument
    /// is the last in the list of arguments.
    const template_argument_spec * next_arg(const template_argument_spec * arg) const {
        return next_arg_impl(this);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        return ast_node::traverse_nodes(arguments(), v);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        return ast_node::traverse_nodes(arguments(), v);
    }

private:
    /// Implementation for prev methods
    template <typename TemplateArgument>
    static TemplateArgument * prev_arg_impl(TemplateArgument * self) {
        constexpr bool is_const = std::is_const_v<TemplateArgument>;
        using template_decl_t = std::conditional_t <
            is_const,
            const template_decl,
            template_decl
        >;

        // getting range of template parameter from parent template node
        assert(self->parent() != nullptr && "template parameter parent is null");
        auto templ = dynamic_cast<template_decl_t*>(self->parent());
        assert(templ && "parent node of template parameter decl is not a template decl");
        auto params = templ->template_params();

        static_assert(std::bidirectional_iterator<decltype(std::ranges::begin(params))>);

        // searching of template parameter in the range of template parameters
        auto it = std::ranges::find(params, self);
        assert(it != std::ranges::end(params) && "can't find template parameter");

        if (it == std::ranges::begin(params)) {
            // this is the first parameter
            return nullptr;
        } else {
            auto prev_it = it;
            --prev_it;
            return *prev_it;
        }
    }

    /// Implementation for next methods
    template <typename TemplateArgument>
    static TemplateArgument * next_arg_impl(TemplateArgument * self) {
        constexpr bool is_const = std::is_const_v<TemplateArgument>;
        using template_decl_t = std::conditional_t <
            is_const,
            const template_decl,
            template_decl
        >;

        // getting range of template parameter from parent template node
        assert(self->parent() != nullptr && "template parameter parent is null");
        auto templ = dynamic_cast<template_decl_t*>(self->parent());
        assert(templ && "parent node of template parameter decl is not a template decl");
        auto params = templ->template_params();

        // searching of template parameter in the range of template parameters
        auto it = std::ranges::find(params, self);
        assert(it != std::ranges::end(params) && "can't find template parameter");

        auto next_it = it;
        ++next_it;
        if (next_it == std::ranges::end(params)) {
            // this is the last parameter
            return nullptr;
        } else {
            return *next_it;
        }
    }


    /// List of template arguments
    std::list<std::unique_ptr<template_argument_spec>> arguments_;
};


}
