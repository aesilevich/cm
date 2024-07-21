// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_substitution.hpp
/// Contains definition of the template_substitution class.

#pragma once

#include "template_argument.hpp"
#include "template_name.hpp"
#include <algorithm>


namespace cm {


/// Represents template substitution: a template name with template arguments
class template_substitution: public entity_use_impl<template_name>,
                             virtual public context_entity {
public:
    /// Constructs template instantiation with pack of template parameters descriptions
    template <std::convertible_to<template_argument_desc> ... Params>
    explicit template_substitution(template_name * templ, Params && ... params):
        template_substitution{templ,
            std::initializer_list<template_argument_desc>{std::forward<Params>(params)...}} {}

    /// Constructs template instantiation with range of template arguments descriptions
    template <template_argument_desc_range Args>
    explicit template_substitution(template_name * templ, Args && t_args):
    entity_use_impl<template_name>{templ},
    context_entity{templ->ctx()} {
        if constexpr (std::ranges::sized_range<Args>) {
            args_.reserve(std::ranges::size(t_args));
        }

        for (auto && arg : t_args) {
            add_arg(arg);
        }
    }

    /// Returns pointer to a template of specified type. Checks that template type matches
    template <std::derived_from<template_name> Template = template_name>
    Template * templ() {
        auto ent = used_entity();
        if constexpr (std::same_as<Template, template_name>) {
            return ent;
        } else {
            auto casted_ent = dynamic_cast<Template*>(ent);
            assert(casted_ent && "invalid template type in substitution");
            return casted_ent;
        }
    }

    /// Returns const pointer to a template of specified type. Checks that template type matches
    template <std::derived_from<template_name> Template = template_name>
    const Template * templ() const {
        auto ent = used_entity();
        if constexpr (std::same_as<Template, template_name>) {
            return ent;
        } else {
            auto casted_ent = dynamic_cast<const Template*>(ent);
            assert(casted_ent && "invalid template type in substitution");
            return casted_ent;
        }
    }

    /// Returns range of const template arguments
    auto args() const {
        auto fn = [](auto && arg) { return const_cast<const template_argument*>(arg.get()); };
        return args_ | std::ranges::views::transform(fn);
    }

    /// Returns range of template arguments
    auto args() {
        auto fn = [](auto && arg) { return arg.get(); };
        return args_ | std::ranges::views::transform(fn);
    }

    /// Adds template instantiation argument
    void add_arg(std::unique_ptr<template_argument> && arg) {
        args_.push_back(std::move(arg));
    }

    /// Adds template instantiation argument from argument description
    void add_arg(const template_argument_desc & arg_desc) {
        if (arg_desc.is_type()) {
            add_arg(std::make_unique<type_template_argument>(this, arg_desc.type()));
        } else {
            add_arg(std::make_unique<value_template_argument>(this, arg_desc.value()));
        }
    }

    /// Removes template argument
    void remove_arg(template_argument * arg) {
        auto ret = std::ranges::remove_if(args_, [arg](auto && arg_uptr) {
            return arg_uptr.get() == arg;
        });

        args_.erase(std::ranges::begin(ret), std::ranges::end(ret));
    }

    /// Returns true if arguments of this instantiation are equal to specified range of arguments
    template <const_template_argument_desc_range Args>
    bool args_equal(Args && args_r) const {
        auto compare_arg = [](const auto * arg, const auto & desc) {
            return arg->desc() == desc;
        };
        return std::ranges::equal(args(), args_r, compare_arg);
    }

    /// Returns true if arguments of this instantiation are equal to specified pack of arguments
    template <std::convertible_to<const_template_argument_desc> ... Args>
    bool args_equal(const Args & ... args) const {
        return args_equal(std::initializer_list<const_template_argument_desc>{args...});
    }

    /// Prints template instantiation description to output stream
    void print_desc(std::ostream & str) const override {
        str << templ()->name() << '<';

        bool first = true;
        for (auto && arg : args()) {
            if (!first) {
                str << ", ";
            } else {
                first = false;
            }

            arg->print_desc(str);
        }

        str << '>';
    }

private:
    /// Vector of template arguments
    std::vector<std::unique_ptr<template_argument>> args_;
};


}
