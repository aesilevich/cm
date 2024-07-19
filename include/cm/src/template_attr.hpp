// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_attr.hpp
/// Contains definition of the template_attr class.

#pragma once

#include "template_spec.hpp"


namespace cm::src {


/// Template specifier AST node attribute
template <typename Derived>
class template_attr {
public:
    /// Returns pointer to template specifier
    template_spec * templ() { return templ_.get(); }

    /// Returns pointer to template specifier
    const template_spec * templ() const { return templ_.get(); }

    /// Sets template specifier
    void set_templ(std::unique_ptr<template_spec> && t) { templ_ = std::move(t); }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        return v.traverse(templ());
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        return v.traverse(templ());
    }

private:
    std::unique_ptr<template_spec> templ_;      ///< Template specifier
};


}
