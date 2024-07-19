// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_scope_spec.hpp
/// Contains definition of classes related to template scope specifiers.

#pragma once

#include "entity_attr.hpp"
#include "scope_spec.hpp"
#include "template_substitution.hpp"


namespace cm::src {


/// Represents abstract template scope specifier
class template_scope_spec: public scope_spec, public template_substitution_spec {
public:
    /// Returns pointer to code model template referenced by this specifier
    virtual template_name * cm_templ() = 0;

    /// Returns const pointer to code model template referenced by this specifier
    virtual const template_name * cm_templ() const = 0;

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) override {
        if (!scope_spec::traverse_children(v)) {
            return false;
        }

        return template_substitution_spec::traverse_children(v);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const override {
        if (!scope_spec::traverse_children(v)) {
            return false;
        }

        return template_substitution_spec::traverse_children(v);
    }
};


using template_record_scope_spec_base = ast_node_impl <
    "template_record_scope_spec",
    template_scope_spec,
    ast_node,
    entity_attr<template_record_type>::type
>;


/// Template record type specifier (injected record). This type is
/// used to represent type of this template record inside template declaration.
class template_record_scope_spec: public template_record_scope_spec_base {
    /// Type of entity attribute of this AST node
    using ent_attr = entity_attr<template_record_type>::type<template_record_scope_spec_base>;

public:
    /// Constructs template record specifier
    explicit template_record_scope_spec(ast_node * parent):
        template_record_scope_spec_base{parent} {}

    /// Returns pointer to code model template referenced by this specifier
    template_name * cm_templ() override {
        return this->ent_attr::entity()->templ();
    }

    /// Returns const pointer to code model template referenced by this specifier
    const template_name * cm_templ() const override {
        return this->ent_attr::entity()->templ();
    }
};


using template_record_instantiation_scope_spec_base = ast_node_impl <
    "template_record_instantiation_scope_spec",
    template_scope_spec,
    ast_node,
    entity_attr<template_record_instantiation_type>::type
>;


/// Template record instantiation scope specifier
class template_record_instantiation_scope_spec:
        public template_record_instantiation_scope_spec_base
{
    /// Type of entity attribute of this AST node
    using ent_attr = entity_attr<template_record_instantiation_type>::type <
        template_record_instantiation_scope_spec_base
    >;

public:
    /// Constructs template record instantiation specifier
    explicit template_record_instantiation_scope_spec(ast_node * parent):
        template_record_instantiation_scope_spec_base{parent} {}

    /// Returns pointer to code model template referenced by this specifier
    template_name * cm_templ() override {
        return this->ent_attr::entity()->templ();
    }

    /// Returns const pointer to code model template referenced by this specifier
    const template_name * cm_templ() const override {
        return this->ent_attr::entity()->templ();
    }
};


}
