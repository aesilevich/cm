// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_substitution.hpp
/// Contains definition of the template substitution related classes.

#pragma once

#include "ast_node_impl.hpp"
#include "template_spec.hpp"


namespace cm::src {


class template_argument_spec;


/// Abstract AST node with template substitution. Contains methods for accessing template name
/// and arguments.
class template_substitution_spec: virtual public ast_node {
public:
    /// Searches for template argument previous to specified. Returns null if previous
    /// template argument was not found (i.e. the specified argument is the first in the list).
    /// NOTE: O(N) complexity
    // virtual template_argument_spec * find_prev_arg(template_argument_spec * arg) = 0;

    // /// Searches for template argument previous to specified. Returns null if previous
    // /// template argument was not found (i.e. the specified argument is the first in the list).
    // /// NOTE: O(N) complexity
    // virtual template_argument_spec * find_prev_arg(template_argument_spec * arg) const = 0;

    // /// Searches for template argument next to specified. Returns null if next
    // /// template argument was not found (i.e. the specified argument is the last in the list).
    // /// NOTE: O(N) complexity
    // virtual template_argument_spec * find_next_arg(template_argument_spec * arg) = 0;

    // /// Searches for template argument next to specified. Returns null if next
    // /// template argument was not found (i.e. the specified argument is the last in the list).
    // /// NOTE: O(N) complexity
    // virtual template_argument_spec * find_next_arg(template_argument_spec * arg) const = 0;


    /// Returns pointer to template specifier
    template_spec * templ() { return templ_.get(); }

    /// Returns pointer to template specifier
    const template_spec * templ() const { return templ_.get(); }

    /// Sets template specifier
    void set_templ(std::unique_ptr<template_spec> && t) { templ_ = std::move(t); }

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

    /// Searches for template argument previous to specified. Returns null if previous
    /// template argument was not found (i.e. the specified argument is the first in the list).
    /// NOTE: O(N) complexity
    template_argument_spec * find_prev_arg(template_argument_spec * arg) {
        return find_prev_arg_impl(this, arg);
    }

    /// Searches for template argument previous to specified. Returns null if previous
    /// template argument was not found (i.e. the specified argument is the first in the list).
    /// NOTE: O(N) complexity
    const template_argument_spec * find_prev_arg(const template_argument_spec * arg) const {
        return find_prev_arg_impl(this, arg);
    }

    /// Searches for template argument next to specified. Returns null if next
    /// template argument was not found (i.e. the specified argument is the last in the list).
    /// NOTE: O(N) complexity
    template_argument_spec * find_next_arg(template_argument_spec * arg) {
        return find_next_arg_impl(this, arg);
    }

    /// Searches for template argument next to specified. Returns null if next
    /// template argument was not found (i.e. the specified argument is the last in the list).
    /// NOTE: O(N) complexity
    const template_argument_spec * find_next_arg(const template_argument_spec * arg) const {
        return find_next_arg_impl(this, arg);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    inline bool traverse_children(ast_visitor & v) override;

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    inline bool traverse_children(const_ast_visitor & v) const override;

private:
    template <typename Self>
    using template_argument_spec_t = std::conditional_t <
        std::is_const_v<Self>,
        const template_argument_spec,
        template_argument_spec
    >;

    /// Implementation for prev methods
    template <typename Self>
    static template_argument_spec_t<Self> *
    find_prev_arg_impl(Self * self, template_argument_spec_t<Self> * arg) {
        // searching of template argument in the range of template parameters
        auto params = self->arguments();
        auto it = std::ranges::find(params, arg);
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
    template <typename Self>
    static template_argument_spec_t<Self> *
    find_next_arg_impl(Self * self, template_argument_spec_t<Self> * arg) {
        // searching of template argument in the range of template parameters
        auto params = self->arguments();
        auto it = std::ranges::find(params, arg);
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


    /// Template specifier
    std::unique_ptr<template_spec> templ_;

    /// List of template arguments
    std::list<std::unique_ptr<template_argument_spec>> arguments_;
};


/// Represents single template argument specification in source code
class template_argument_spec: virtual public ast_node {
public:
    /// Returns pointer to parent template substitution
    virtual template_substitution_spec * parent() = 0;

    /// Returns const pointer to parent template substitution
    virtual const template_substitution_spec * parent() const = 0;

    /// Searches for template argument previous to this. Returns null if previous
    /// template argument was not found (i.e. this argument is the first in the list).
    /// NOTE: O(N) complexity
    template_argument_spec * find_prev() {
        return parent()->find_prev_arg(this);
    }

    /// Searches for template argument previous to this. Returns null if previous
    /// template argument was not found (i.e. this argument is the first in the list).
    /// NOTE: O(N) complexity
    const template_argument_spec * find_prev() const {
        return parent()->find_prev_arg(this);
    }

    /// Searches for template argument next to this. Returns null if next
    /// template argument was not found (i.e. this argument is the last in the list).
    /// NOTE: O(N) complexity
    template_argument_spec * find_next() {
        return parent()->find_next_arg(this);
    }

    /// Searches for template argument next to this. Returns null if next
    /// template argument was not found (i.e. this argument is the last in the list).
    /// NOTE: O(N) complexity
    const template_argument_spec * find_next() const {
        return parent()->find_next_arg(this);
    }
};


/// Represents type template argument specification in source code
using type_template_argument_spec = ast_node_impl <
    "type_template_argument_spec",
    template_argument_spec,
    template_substitution_spec,
    entity_attr<type_template_argument>::type,
    type_attr
>;


/// Represents value template argument specification in source code
using value_template_argument_spec = ast_node_impl <
    "value_template_argument_spec",
    template_argument_spec,
    template_substitution_spec,
    entity_attr<value_template_argument>::type
>;


//////////////////////////////////////////////////////////////////////
// IMPLEMENTATION


inline bool template_substitution_spec::traverse_children(ast_visitor & v) {
    if (!v.traverse(templ())) {
        return false;
    }

    return traverse_nodes(arguments(), v);
}


inline bool template_substitution_spec::traverse_children(const_ast_visitor & v) const {
    if (!v.traverse(templ())) {
        return false;
    }

    return traverse_nodes(arguments(), v);
}


}
