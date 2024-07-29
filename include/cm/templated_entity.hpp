// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file templated_entity.hpp
/// Contains definition of the templated_entity class.

#pragma once


namespace cm {


class templated_entity;


/// Represents template parameter definition
class template_parameter: virtual public named_context_entity {
public:
    /// Constructs template parameter with specified name
    template_parameter(context * ctx, const std::string & nm):
        named_context_entity{ctx, nm},
        context_entity{ctx} {}

    /// Destroys object
    virtual ~template_parameter() = default;

    /// Returns pointer to parent template
    templated_entity * templ();

    /// Returns const pointer to parent template
    const templated_entity * templ() const;
};


/// Type template parameter
class type_template_parameter: public template_parameter, public type_t {
public:
    /// Constructs type template parameter with specified name
    explicit type_template_parameter(context * ctx, const std::string & nm):
        template_parameter{ctx, nm},
        named_context_entity{ctx, nm},
        context_entity{ctx} {}
    
    /// Returns name of context_entity
    const std::string & name() const override {
        return template_parameter::name();
    }

    /// Dumps template parameter to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        str << "type " << name();
        context_entity::dump_loc(str, opts);
    }
};


/// Represents value template parameter definition
class value_template_parameter: public template_parameter, virtual public named_entity {
public:
    /// Constructs value template parameter of specified type
    value_template_parameter(context * ctx, const std::string & nm, type_t * t):
        template_parameter(ctx, nm),
        named_context_entity{ctx, nm},
        context_entity{ctx},
        type_{t} {}

    /// Returns type of value parameters
    type_t * type() { return type_; }

    /// Returns type of value parameter
    const type_t * type() const { return type_; }

    /// Dumps template parameter to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        str << "var " << name() << ": ";
        type()->print_desc(str);
        context_entity::dump_loc(str, opts);
    }

private:
    type_t * type_;         ///< Type of parameter
};


/// Unique pointer to template parameter
using template_parameter_up = std::unique_ptr<template_parameter>;


/// Represents some kind of templated entity in code model which has template
/// parameters: templates itself or partial template specializations.
class templated_entity: virtual public context {
public:
    /// Constructs templated entity with no parameters
    templated_entity(context * ctx):
        context{ctx} {}

    /// Default virtual destructor
    virtual ~templated_entity() = default;

    /// Returns range of tempalte parameters
    auto template_params() {
        return entities<template_parameter>();
    }

    /// Returns const range of template parameters
    auto template_params() const {
        return entities<template_parameter>();
    }

    /// Returns template parameter with specified index
    template_parameter * template_param(size_t idx) {
        auto params = template_params();
        assert(idx < std::ranges::distance(params) && "invalid template parameter index");
        auto it = std::ranges::begin(params);
        std::ranges::advance(it, idx);
        return *it;
    }

    /// Returns template parameter with specified index
    const template_parameter * template_param(size_t idx) const {
        auto params = template_params();
        assert(idx < std::ranges::distance(params) && "invalid template parameter index");
        auto it = std::ranges::begin(params);
        std::ranges::advance(it, idx);
        return *it;
    }

    /// Returns true if template is variadic
    bool is_variadic() const {
        return is_variadic_;
    }

    /// Sets whether template is variadic
    void set_is_variadic(bool is_var) {
        is_variadic_ = is_var;
    }

    /// Adds type template parameter with specifeid name
    type_template_parameter * add_type_template_param(const std::string & nm) {
        return create_named_entity<type_template_parameter>(nm);
    }

    /// Adds value template parameter with specified name and type
    value_template_parameter * add_value_template_param(const std::string & nm, type_t * t) {
        return create_named_entity<value_template_parameter>(nm, t);
    }

    /// Dumps template parameters to output stream
    void dump_template_params(std::ostream & str, const dump_options & opts) const {
        str << '<';

        bool first = true;
        for (auto && par : template_params()) {
            if (!first) {
                str << ", ";
            } else {
                first = false;
            }

            par->dump(str, opts, 0);
        }

        str << '>';
    }

    /// Adds template parameters
    void add_params(auto && ... params) {
        add_params_impl(std::forward<decltype(params)>(params)...);
    }

private:
    // /// Adds template parameter
    // void add_param(template_parameter_up && par) {
    //     params_.push_back(std::move(par));
    // }

    // Helper functions for adding template parameters from constructor with
    // variadic arguments

    void add_params_impl() {}

    template <typename ... Params>
    void add_params_impl(const std::string & par, Params && ... params) {
        add_type_template_param(par);
        add_params_impl(std::forward<Params>(params)...);
    }

    template <typename ... Params>
    void add_params_impl(const std::tuple<std::string, type_t*> & par, Params && ... params) {
        add_value_template_param(std::get<0>(par), std::get<1>(par));
        add_params_impl(std::forward<Params>(params)...);
    }


    bool is_variadic_ = false;                      ///< True if last template parameter is parameter pack
};



inline templated_entity * template_parameter::templ() {
    assert(ctx() != nullptr && "context is null for template parameter");
    auto res = dynamic_cast<templated_entity*>(ctx());
    assert(res && "parent context for template parameter is not a template");
    return res;
}


inline const templated_entity * template_parameter::templ() const {
    assert(ctx() != nullptr && "context is null for template parameter");
    auto res = dynamic_cast<const templated_entity*>(ctx());
    assert(res && "parent context for template parameter is not a template");
    return res;
}


}
