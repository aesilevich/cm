// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_decl.hpp
/// Contains definition of the template_decl class.

#pragma once

#include "declaration.hpp"
#include "entity_attr.hpp"
#include "type_decl.hpp"
#include "variable_decl.hpp"


namespace cm::src {


using template_parameter_decl_base = intermediate_ast_node_impl <
    declaration,
    name_attr
>;


/// Template parameter declaration
class template_parameter_decl: public template_parameter_decl_base {
public:
    /// Returns pointer to declaration of previous template parameter or null if this
    /// template parameter is the first one
    template_parameter_decl * prev();

    /// Returns const pointer to declaration of previous template parameter of null if this
    /// template parameter is the first one
    const template_parameter_decl * prev() const;

    /// Returns pointer to declaration of next template parameter or null if this template
    /// parameter is the last one
    template_parameter_decl * next();

    /// Returns const pointer to declaration of next template parameter or null if this template
    /// parameter is the last one
    const template_parameter_decl * next() const;

private:
    /// Implementation for prev methods
    template <typename TemplateParameter>
    static TemplateParameter * prev_impl(TemplateParameter * self);

    /// Implementation for next methods
    template <typename TemplateParameter>
    static TemplateParameter * next_impl(TemplateParameter * self);
};


/// Type template parameter declaration
using type_template_parameter_decl = ast_node_impl <
    "type_template_parameter_decl",
    template_parameter_decl,
    ast_node,
    entity_attr<type_template_parameter>::type
>;

/// Value template parameter declaration
using value_template_parameter_decl = ast_node_impl <
    "value_template_parameter_decl",
    template_parameter_decl,
    ast_node,
    entity_attr<value_template_parameter>::type,
    type_attr
>;


using template_decl_base = ast_node_impl <
    "template",
    declaration,
    ast_node
>;

/// Template declaration in source code
class template_decl: public template_decl_base {
public:
    /// Constructs template declaration
    template_decl(ast_node * parent):
        template_decl_base{parent} {}

    /// Returns pointer to templated delcaration
    declaration * templated_decl() { return decl_.get(); }

    /// Returns pointer to templated delcaration
    const declaration * templated_decl() const { return decl_.get(); }

    /// Sets templated declaration
    void set_templated_decl(std::unique_ptr<declaration> && d) {
        decl_ = std::move(d);
    }

    /// Creates templated declaration of specified type
    template <std::derived_from<declaration> Declaration>
    Declaration * create_templated_decl() {
        auto decl = std::make_unique<Declaration>(this);
        auto decl_ptr = decl.get();
        set_templated_decl(std::move(decl));
        return decl_ptr;
    }

    /// Returns reference to list of tempalte parameters
    auto & template_params_list() { return params_; }

    /// Returns range of template parameters
    auto template_params() {
        auto fn = [](auto && uptr) { return uptr.get(); };
        return params_ | std::ranges::views::transform(fn);
    }

    /// Returns const range of template arguments
    auto template_params() const {
        auto fn = [](auto && uptr) {
            return const_cast<const template_parameter_decl*>(uptr.get());
        };

        return params_ | std::ranges::views::transform(fn);        
    }

    /// Adds template parameter to the list of parameters
    void add_param(std::unique_ptr<template_parameter_decl> && par) {
        template_params_list().push_back(std::move(par));
    }

    /// Creates new type template parameter and adds it to the list of parameters
    type_template_parameter_decl * add_type_param(type_template_parameter * par) {
        auto par_decl = std::make_unique<type_template_parameter_decl>(this);
        auto par_decl_ptr = par_decl.get();
        par_decl->set_entity(par);
        add_param(std::move(par_decl));
        return par_decl_ptr;
    }

    /// Creates new value template parameter and adds it to the list of parameters
    value_template_parameter_decl * add_value_param(value_template_parameter * par) {
        auto par_decl = std::make_unique<value_template_parameter_decl>(this);
        auto par_decl_ptr = par_decl.get();
        par_decl->set_entity(par);
        add_param(std::move(par_decl));
        return par_decl_ptr;
    }

    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(ast_visitor & v) override {
        return traverse_children_impl(*this, v);
    }

    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(const_ast_visitor & v) const override {
        return traverse_children_impl(*this, v);
    }

private:
    /// Traverse implementation
    template <typename Self, typename Visitor>
    static bool traverse_children_impl(Self & self, Visitor & v) {
        // traversing template parameters
        if (!traverse_nodes(self.template_params(), v)) {
            return false;
        }

        // traversing templated declaration
        return v.traverse(self.templated_decl());
    };


    std::unique_ptr<declaration> decl_;
    std::list<std::unique_ptr<template_parameter_decl>> params_;
};


}
