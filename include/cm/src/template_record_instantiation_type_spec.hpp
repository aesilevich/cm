// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_record_instantiation_type_spec.hpp
/// Contains definition of the template_record_instantiation_type_spec class.

#pragma once

#include "entity_attr.hpp"
#include "template_substitution.hpp"
#include "type_spec.hpp"
#include "../cm.hpp"


namespace cm::src {


/// Template substitution type specifier
class template_substitution_type_spec: public type_spec, public template_substitution_spec {
public:
    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) override {
        if (!type_spec::traverse_children(v)) {
            return false;
        }

        return template_substitution_spec::traverse_children(v);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const override {
        if (!type_spec::traverse_children(v)) {
            return false;
        }

        return template_substitution_spec::traverse_children(v);
    }
};


/// Template record instantiation type specifier
using template_record_instantiation_type_spec = ast_node_impl <
    "template_record_instantiation_type_spec",
    template_substitution_type_spec,
    ast_node,
    entity_attr<template_record_instantiation_type>::type
>;


/// Template record type specifier (i.e. template record itself in template definition)
using template_record_type_spec = ast_node_impl <
    "template_record_type_spec",
    template_substitution_type_spec,
    ast_node,
    entity_attr<template_record_type>::type
>;


}
