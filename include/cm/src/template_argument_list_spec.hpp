// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_argument_list_spec.hpp
/// Contains definition of the template_argument_list_spec type.

#pragma once

#include "template_substitution.hpp"
#include <list>


namespace cm::src {


/// Represents template argument list specification in source code
class template_argument_list_spec {
public:
    /// Constructs empty list of template arguments
    template_argument_list_spec() = default;

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

    /// Traverses all arguments. Returns true if visiting should continue after the call.
    bool visit_arguments(ast_visitor & v) {
        return ast_node::traverse_nodes(arguments(), v);
    }

    /// Traverses all arguments. Returns true if visiting should continue after the call.
    bool visit_arguments(const_ast_visitor & v) const {
        return ast_node::traverse_nodes(arguments(), v);
    }

private:
    /// List of template arguments
    std::list<std::unique_ptr<template_argument_spec>> arguments_;
};


}
