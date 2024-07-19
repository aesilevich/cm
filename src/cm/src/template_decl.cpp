// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_decl.cpp
/// Contains implementation of template declaration related classes.

#include "pch.hpp"
#include "cm/src/template_decl.hpp"


namespace cm::src {


template_parameter_decl * template_parameter_decl::prev() {
    return prev_impl(this);
}


const template_parameter_decl * template_parameter_decl::prev() const {
    return prev_impl(this);
}


template_parameter_decl * template_parameter_decl::next() {
    return next_impl(this);
}


const template_parameter_decl * template_parameter_decl::next() const {
    return next_impl(this);
}


template <typename TemplateParameter>
TemplateParameter * template_parameter_decl::prev_impl(TemplateParameter * self) {
    constexpr bool is_const = std::is_const_v<TemplateParameter>;
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


template <typename TemplateParameter>
TemplateParameter * template_parameter_decl::next_impl(TemplateParameter * self) {
    constexpr bool is_const = std::is_const_v<TemplateParameter>;
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


}
