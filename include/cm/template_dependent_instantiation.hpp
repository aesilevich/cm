// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_dependent_instantiation.hpp
/// Contains definition of the template_dependent_instantiation class.

#pragma once

#include "template_substitution.hpp"


namespace cm {


/// Template instantiation dependent from template parameters in some template context.
class template_dependent_instantiation: virtual public template_substitution,
                                        virtual public context_type {
public:
    /// Constructs template instantiation with pack of template arguments descriptions
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_dependent_instantiation(template_ * templ, Args && ... args):
        template_substitution{templ, std::forward<Args>(args)...},
        context_type{templ->ctx()},
        context_entity{templ->ctx()} {}

    /// Constructs template instantiation with range of template arguments descriptions
    template <template_argument_desc_range Args>
    explicit template_dependent_instantiation(template_ * templ, Args && args):
        template_substitution{templ, std::forward<Args>(args)},
        context_type{templ->ctx()},
        context_entity{templ->ctx()} {}
};


}
