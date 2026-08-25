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
class template_template_parameter;


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

    /// Returns true if template parameter is a parameter pack
    bool is_variadic() const {
        return is_variadic_;
    }

    /// Sets whether template parameter is a parameter pack
    void set_is_variadic(bool is_var) {
        is_variadic_ = is_var;
    }

private:
    bool is_variadic_ = false;      ///< True if template parameter is a parameter pack
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
        str << "type";
        if (is_variadic()) {
            str << " ...";
        }

        if (!name().empty()) {
            str << ' ' << name();
        }

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
        str << "var";
        if (is_variadic()) {
            str << " ...";
        }

        if (!name().empty()) {
            str << ' ' << name();
        }

        str << ": ";
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

    /// Adds type template parameter with specifeid name
    type_template_parameter * add_type_template_param(const std::string & nm) {
        return create_named_entity<type_template_parameter>(nm);
    }

    /// Adds template template parameter with specified name
    inline template_template_parameter * add_template_template_param(const std::string & nm);

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
};



/// Template template parameter
class template_template_parameter: public template_parameter,
                                   public templated_entity,
                                   public template_name {
public:
    /// Constructs template template parameter with specified name
    explicit template_template_parameter(context * ctx, const std::string & nm):
        template_parameter{ctx, nm},
        templated_entity{ctx},
        named_context_entity{ctx, nm},
        context{ctx},
        context_entity{ctx} {}

    /// Returns name of context_entity
    const std::string & name() const override {
        return template_parameter::name();
    }

    /// Returns default access level for this context
    access_level default_access_level() const override {
        return access_level::public_;
    }

    /// Dumps template parameter to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        str << "template ";
        dump_template_params(str, opts);

        if (is_variadic()) {
            str << " ...";
        }

        if (!name().empty()) {
            str << ' ' << name();
        }

        context_entity::dump_loc(str, opts);
    }
};


inline template_template_parameter *
templated_entity::add_template_template_param(const std::string & nm) {
    return create_named_entity<template_template_parameter>(nm);
}


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
