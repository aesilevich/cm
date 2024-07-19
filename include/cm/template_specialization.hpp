// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_specialization.hpp
/// Contains definition of the template_specialization class.

#pragma once

#include "template_instantiation.hpp"


namespace cm {


/// User defined template specialization
class template_specialization: virtual public template_instantiation {
public:
    /// Constructs template specialization with pack of template arguments descriptions
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_specialization(context * ctx, template_ * templ, Args && ... args):
        template_instantiation{templ, std::forward<Args>(args)...},
        template_substitution{templ, std::forward<Args>(args)...},
        context_entity{ctx} {}

    /// Constructs template specialization with range of template arguments descriptions
    template <template_argument_desc_range Args>
    explicit template_specialization(context * ctx, template_ * templ, Args && args):
        template_instantiation{templ, std::forward<Args>(args)},
        template_substitution{templ, std::forward<Args>(args)},
        context_entity{ctx} {}
};


}
